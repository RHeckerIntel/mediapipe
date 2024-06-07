#include "graph_runner.h"
#include <iostream>

#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/canonical_errors.h"

namespace geti{

void GraphRunner::OpenGraph(const char* graph_content) {
    mediapipe::CalculatorGraphConfig graph_config = mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(absl::Substitute(graph_content));
    graph = std::make_shared<mediapipe::CalculatorGraph>(graph_config);

    poller = std::unique_ptr<mediapipe::OutputStreamPoller>(new mediapipe::OutputStreamPoller(graph->AddOutputStreamPoller("output").value()));
    graph->StartRun({});
    running = true;
    std::cout << "Started..." << std::endl;
}

std::string GraphRunner::Get() {
    mediapipe::Packet output_packet;
    std::cout << "getting..." << std::endl;
    std::cout << graph->HasError() << std::endl;
    if (running && poller->Next(&output_packet)) {
        return output_packet.Get<std::string>();
    }
    return "";
}


void GraphRunner::Listen(const std::function<void(const std::string&)> callback) {
    mediapipe::Packet output_packet;
    while (running && poller->Next(&output_packet)) {
        callback(output_packet.Get<std::string>());
    }
}

void GraphRunner::Queue(const std::vector<char>& image_data) {
    std::cout << "queued..." << std::endl;
    auto image = cv::imdecode(image_data, 1);
    auto packet = mediapipe::MakePacket<cv::Mat>(image).At(mediapipe::Timestamp(++timestamp));
    graph->AddPacketToInputStream("input", packet);
}

void GraphRunner::Queue(const std::string& input) {
    std::cout << "queued..." << std::endl;
    std::cout << input << std::endl;
    auto packet = mediapipe::MakePacket<std::string>(input).At(mediapipe::Timestamp(++timestamp));
    std::cout << "created packet" << std::endl;
    graph->AddPacketToInputStream("input", packet);
}

void GraphRunner::Stop() {
    graph->CloseAllInputStreams();
    graph->WaitUntilDone();
    graph->Cancel();
    //graph->WaitUntilIdle();
    running = false;
}

void GraphRunner::SetupLogging(const char* filename) {
    FLAGS_alsologtostderr = true;
    google::SetLogDestination(google::GLOG_ERROR,filename);
    if (!google::IsGoogleLoggingInitialized()) {
        google::InitGoogleLogging("GraphRunner");
    }
}

}
