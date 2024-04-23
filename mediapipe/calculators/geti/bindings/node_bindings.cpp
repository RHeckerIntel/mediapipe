#include <models/detection_model.h>
#include <models/input_data.h>
#include <models/results.h>

#include <chrono>
#include <fstream>
#include <memory>
#include <opencv4/opencv2/core/mat.hpp>
#include <stdexcept>
#include <thread>

#include "inference/kserve.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "node/graphs.h"
#include "serialization/result_serialization.h"
#include "third_party/napi/napi.h"

constexpr float kMicrosPerSecond = 1e6;

template<typename T>
static T runSingleFireGraph(const mediapipe::CalculatorGraphConfig& graph_config, const std::map<std::string, mediapipe::Packet>& inputSidePackets, const std::map<std::string, mediapipe::Packet>& inputs) {
  mediapipe::CalculatorGraph graph(geti::detection_graph_config);

  mediapipe::OutputStreamPoller output_image_poller =
      graph.AddOutputStreamPoller("output").value();

  graph.StartRun(inputSidePackets);

  for (const auto& input: inputs) {
    graph.AddPacketToInputStream(input.first, input.second.At(mediapipe::Timestamp(0)));
  }

  graph.WaitUntilIdle();
  mediapipe::Packet output_packet;
  output_image_poller.Next(&output_packet);
  return output_packet.Get<T>();
}

class SingleFireGraphWorker : public Napi::AsyncWorker {
 public:
  Napi::Promise::Deferred deferred;
  std::string model_path;
  cv::Mat input_image;
  std::string output;
  const mediapipe::CalculatorGraphConfig &graph_config;
  SingleFireGraphWorker(Napi::Env& env, std::string model_path, cv::Mat input_image, const mediapipe::CalculatorGraphConfig& graph_config) : Napi::AsyncWorker(env), deferred(env), model_path(model_path), input_image(input_image), graph_config(graph_config) {}
  void Execute() {
    std::map<std::string, mediapipe::Packet> inputSidePackets;
    inputSidePackets["model_path"] = mediapipe::MakePacket<std::string>(model_path).At(mediapipe::Timestamp(0));
    inputSidePackets["device"] = mediapipe::MakePacket<std::string>("AUTO").At(mediapipe::Timestamp(0));

    std::map<std::string, mediapipe::Packet> packets;
    packets["input"] = mediapipe::MakePacket<cv::Mat>(input_image);

    auto output_image = runSingleFireGraph<cv::Mat>(graph_config, inputSidePackets, packets);
    output = geti::base64_encode_mat(output_image);
  }

  void OnOK() {
    deferred.Resolve(Napi::String::New(Env(), output));
  }

  void OnError(Napi::Error const &error) {
      deferred.Reject(error.Value());
  }

};


class GraphOutputWorker : public Napi::AsyncProgressWorker<std::string> {
 public:
  std::shared_ptr<mediapipe::OutputStreamPoller> poller;
  GraphOutputWorker(Napi::Function& callback, std::shared_ptr<mediapipe::OutputStreamPoller> poller) : Napi::AsyncProgressWorker<std::string>(callback), poller(poller) {}
  void Execute(const ExecutionProgress & progress) {
    mediapipe::Packet output_packet;
    std::cout << "listening..." << std::endl;
    while(running && poller->Next(&output_packet)) {
      const auto& output_image = output_packet.Get<cv::Mat>();

      const auto msg = geti::base64_encode_mat(output_image);
      progress.Send(&msg, 1);
    }

    std::cout << "listening ended" << std::endl;
  }

  void OnOK() {
  }

  void OnProgress(const std::string* data, size_t /* count */) {
    Napi::HandleScope scope(Env());

    Callback().Call({Env().Null(), Napi::String::New(Env(), *data)});
  }


  void OnError(Napi::Error const &error) {
  }

  bool running = true;

};

static const mediapipe::CalculatorGraphConfig& get_graph_config_by_task(const std::string& task_name) {

  if (task_name == "detection") {
    return geti::detection_graph_config;
  }

  throw std::runtime_error("No graph available by name: '" + task_name + "'");
}

static Napi::Promise execute(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  const std::string task_name = info[0].As<Napi::String>();
  const std::string model_path = info[1].As<Napi::String>();
  const auto image_buffer = info[2].As<Napi::Buffer<char>>();

  const std::vector<char> image_data(
      image_buffer.Data(), image_buffer.Data() + image_buffer.Length());
  auto image = cv::imdecode(image_data, 1);

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  try{
    const auto& graph_config = get_graph_config_by_task(task_name);
    SingleFireGraphWorker* asyncWorker = new SingleFireGraphWorker(env, model_path, image, graph_config);
    asyncWorker->Queue();

    return asyncWorker->deferred.Promise();

  } catch(std::runtime_error err) {
    auto deferred = Napi::Promise::Deferred(env);
    deferred.Reject(Napi::String::New(env, err.what()));
    return deferred.Promise();
  }

}



static Napi::String graph(const Napi::CallbackInfo& info) {
  const std::string graph_input = info[0].As<Napi::String>();
  const std::string model_path = info[1].As<Napi::String>();
  const auto image_buffer = info[2].As<Napi::Buffer<char>>();

  const std::vector<char> image_data(
      image_buffer.Data(), image_buffer.Data() + image_buffer.Length());
  auto image = cv::imdecode(image_data, 1);

  mediapipe::CalculatorGraph graph(geti::detection_graph_config);

  std::map<std::string, mediapipe::Packet> inputSidePackets;
  inputSidePackets["model_path"] =
      mediapipe::MakePacket<std::string>(model_path)
          .At(mediapipe::Timestamp(0));
  inputSidePackets["device"] =
      mediapipe::MakePacket<std::string>("AUTO").At(mediapipe::Timestamp(0));

  mediapipe::OutputStreamPoller output_image_poller =
      graph.AddOutputStreamPoller("output").value();

  graph.StartRun(inputSidePackets);

  const size_t fake_timestamp_us = static_cast<double>(cv::getTickCount()) /
                                   static_cast<double>(cv::getTickFrequency()) *
                                   kMicrosPerSecond;

  graph.AddPacketToInputStream("input",
                               mediapipe::MakePacket<cv::Mat>(image).At(
                                   mediapipe::Timestamp(fake_timestamp_us)));

  graph.WaitUntilIdle();

  mediapipe::Packet output_packet;
  std::cout << output_image_poller.Next(&output_packet) << std::endl;
  auto& output_image = output_packet.Get<cv::Mat>();

  Napi::Env env = info.Env();

  return Napi::String::New(env, geti::base64_encode_mat(output_image));
}

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "graph"), Napi::Function::New(env, graph));
  exports.Set(Napi::String::New(env, "worker"),
              Napi::Function::New(env, execute,
                                  std::string("runSimpleAsyncWorker")));

  // exports.Set(Napi::String::New(env, "graph"), Napi::Function::New(env,
  // graph));
  return exports;
}

//NODE_API_MODULE(hello, Init)

class ExampleAddon : public Napi::Addon<ExampleAddon> {
 public:
  ExampleAddon(Napi::Env env, Napi::Object exports): deferred(env), graph(std::make_shared<mediapipe::CalculatorGraph>()) {
    // In the constructor we declare the functions the add-on makes available
    // to JavaScript.
    DefineAddon(exports, {
      InstanceMethod("open_graph", &ExampleAddon::OpenGraph),
      InstanceMethod("queue", &ExampleAddon::Queue),
      InstanceMethod("listen", &ExampleAddon::Listen),
    });

  }
 private:

  Napi::Value OpenGraph(const Napi::CallbackInfo& info) {
    const std::string task_name = info[0].As<Napi::String>();
    const std::string model_path = info[1].As<Napi::String>();

    std::map<std::string, mediapipe::Packet> inputSidePackets;
    inputSidePackets["model_path"] =
        mediapipe::MakePacket<std::string>(model_path)
            .At(mediapipe::Timestamp(0));
    inputSidePackets["device"] =
        mediapipe::MakePacket<std::string>("AUTO").At(mediapipe::Timestamp(0));

    const auto& graph_config = get_graph_config_by_task(task_name);
    graph = std::make_shared<mediapipe::CalculatorGraph>(graph_config);

    poller = std::unique_ptr<mediapipe::OutputStreamPoller>(new mediapipe::OutputStreamPoller(graph->AddOutputStreamPoller("output").value()));
    //graph->Initialize(graph_config);
    graph->StartRun(inputSidePackets);

    return Napi::Boolean::New(info.Env(), true);
  }

  Napi::Value Listen(const Napi::CallbackInfo& info) {
    Napi::Function callback = info[0].As<Napi::Function>();
    GraphOutputWorker* asyncWorker = new GraphOutputWorker(callback, poller);
    asyncWorker->Queue();
    return Napi::Boolean::New(info.Env(), true);
  }

  Napi::Value Queue(const Napi::CallbackInfo& info) {
    const auto image_buffer = info[0].As<Napi::Buffer<char>>();

    std::cout << "queuing img" << std::endl;

    const std::vector<char> image_data(
        image_buffer.Data(), image_buffer.Data() + image_buffer.Length());
    auto image = cv::imdecode(image_data, 1);
    auto packet = mediapipe::MakePacket<cv::Mat>(image).At(mediapipe::Timestamp(++timestamp));
    graph->AddPacketToInputStream("input", packet);

    std::cout << "queued..." << std::endl;

    return deferred.Promise();
  }

  // Data stored in these variables is unique to each instance of the add-on.
  int64 timestamp = 0;
  std::shared_ptr<mediapipe::OutputStreamPoller> poller;
  std::shared_ptr<mediapipe::CalculatorGraph> graph;
  Napi::Promise::Deferred deferred;
};

NODE_API_ADDON(ExampleAddon);
