#include "graph_runner.h"
#include <iostream>
#include "utils.h"

namespace geti{

bool GraphRunner::OpenGraph(const std::string& task_name, const std::string& model_path) {
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
    running = true;

    std::cout << "Started..." << std::endl;

    return true;
}

std::string GraphRunner::Get() {
    mediapipe::Packet output_packet;
    if (running && poller->Next(&output_packet)) {
        std::cout << " whoop new package " << std::endl;
        cv::Mat output_image = output_packet.Get<cv::Mat>();
        std::cout << "huh?" << std::endl;
        cv::imwrite("C:/data/output.png", output_image);
        return geti::base64_encode_mat(output_image);
    }
    return "";
}

void GraphRunner::Queue(const std::vector<char>& image_data) {
    std::cout << "queuing img" << std::endl;

    std::cout << image_data.size() << std::endl;
    auto image = cv::imdecode(image_data, 1);
    std::cout << image.size() << std::endl;
    auto packet = mediapipe::MakePacket<cv::Mat>(image).At(mediapipe::Timestamp(++timestamp));
    graph->AddPacketToInputStream("input", packet);

    std::cout << "queued..." << std::endl;
}

}
