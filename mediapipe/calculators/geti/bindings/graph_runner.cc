#include "graph_runner.h"
#include <iostream>

#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/canonical_errors.h"
#include <chrono>
#include <thread>

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
    std::cout << "Has error: " << graph->HasError() << std::endl;
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
    std::cout << "End of listening" << std::endl;
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
    std::cout << "GraphRunner::Stop" << std::endl;
    running = false;
    camera_running = false;
    if (camera_thread.joinable()) {
        camera_thread.join();
    }
    graph->CloseAllInputStreams();
    graph->WaitUntilDone();
    graph->Cancel();
    //graph->WaitUntilIdle();
}

void GraphRunner::SetupLogging(const char* filename) {
    FLAGS_alsologtostderr = true;
    google::SetLogDestination(google::GLOG_ERROR,filename);
    if (!google::IsGoogleLoggingInitialized()) {
        google::InitGoogleLogging("GraphRunner");
    }
}

bool GraphRunner::OpenCamera(const int &device) {
    std::cout << "Loading camera: " << device << std::endl;
    cv::VideoCapture cap;
    camera_running = true;
    std::cout << device << std::endl;
    cap.open(device);
    if (!cap.isOpened()) {
        LOG(ERROR) << "ERROR! Unable to open camera\n";
        return false;
    }

    cv::Mat frame;
    mediapipe::Packet output_packet;
    while(camera_running) {
        std::cout << "input..." << std::endl;
        cap.read(frame);
        std::cout << frame.rows << std::endl;
        if (frame.empty()) {
            std::cout << "empty frame" << std::endl;
            graph->WaitUntilIdle();
            continue;
        }
        auto packet = mediapipe::MakePacket<cv::Mat>(frame).At(mediapipe::Timestamp(++timestamp));
        graph->AddPacketToInputStream("input", packet);
        graph->WaitUntilIdle();
    }

    std::cout << "Camera done." << std::endl;

    return true;


}

void GraphRunner::StopCamera() {
    camera_running = false;
    if (camera_thread.joinable()) {
        camera_thread.join();
    }
}

}
