#include "graph_runner.h"
#include <iostream>

#include "mediapipe/framework/port/parse_text_proto.h"

namespace geti{

bool GraphRunner::OpenGraph(const std::string& graph_content) {
    mediapipe::CalculatorGraphConfig graph_config = mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(absl::Substitute(graph_content));
    graph = std::make_shared<mediapipe::CalculatorGraph>(graph_config);

    poller = std::unique_ptr<mediapipe::OutputStreamPoller>(new mediapipe::OutputStreamPoller(graph->AddOutputStreamPoller("output").value()));
    graph->StartRun({});
    running = true;

    std::cout << "Started..." << std::endl;

    return true;
}

std::string GraphRunner::Get() {
    mediapipe::Packet output_packet;
    if (running && poller->Next(&output_packet)) {
        return output_packet.Get<std::string>();
    }
    return "";
}

void GraphRunner::Listen(const std::function<void(const std::string&)> callback) {
    mediapipe::Packet output_packet;
    while (running && poller->Next(&output_packet)) {
        std::cout << " whoop new package " << std::endl;
        callback(output_packet.Get<std::string>());
    }
}

void GraphRunner::Queue(const std::vector<char>& image_data) {
    std::cout << "queued..." << std::endl;
    auto image = cv::imdecode(image_data, 1);
    std::cout << "image properties: " << image.size() << std::endl;
    auto packet = mediapipe::MakePacket<cv::Mat>(image).At(mediapipe::Timestamp(++timestamp));
    graph->AddPacketToInputStream("input", packet);
}

void GraphRunner::Stop() {
    graph->Cancel();
    //graph->WaitUntilIdle();
    running = false;
}

}
