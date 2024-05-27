#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>


#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/parse_text_proto.h"

absl::Status startConversation(const std::string& graph_path) {
  std::string calculator_graph_config_contents;
  mediapipe::file::GetContents(graph_path, &calculator_graph_config_contents);

  mediapipe::CalculatorGraphConfig graph_config =
    mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(calculator_graph_config_contents);

  graph_config.set_max_queue_size(10);

  std::unique_ptr<mediapipe::CalculatorGraph> graph =
      absl::make_unique<mediapipe::CalculatorGraph>();
  MP_RETURN_IF_ERROR(graph->Initialize(graph_config));
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller, graph->AddOutputStreamPoller("output"));
  //graph->ObserveOutputStream("output", [&](const mediapipe::Packet& packet) {
  //  std::cout << packet.Timestamp() << ": " << packet.Get<std::string>() << std::endl;
  //  return absl::OkStatus();
  //});

  graph->StartRun({});
  //graph->

  std::string prompt = "What color is the sky?";
  mediapipe::Timestamp timestamp(1);
//  while(true) {
//      std::cout << "User: ";
//      getline(std::cin, prompt);
//
//std::cout << prompt;



  const std::string SPECIAL_EOS_WORD = "</s>";
  while(true) {
    std::cout << "User: ";
    getline(std::cin, prompt);

    std::string word = "";
    while(word != SPECIAL_EOS_WORD) {
      graph->AddPacketToInputStream("input", mediapipe::MakePacket<std::string>(prompt).At(timestamp));
      mediapipe::Packet packet;
      poller.Next(&packet);
      word = packet.Get<std::string>();
      std::cout << word.size() << std::endl;
      std::cout << word << std::endl;
      prompt = "";
      ++timestamp;
    }
  }

  std::cout << "Done" << std::endl;

  graph->CloseAllInputStreams();
  graph->WaitUntilIdle();

  return absl::OkStatus();
}

int main(int argc, char** argv) {
  if (argc != 2) {
    throw std::runtime_error(std::string{"Usage: "} + argv[0] + " <path_to_graph>");
  }
  std::string graph_path = argv[1];
  auto status = startConversation(graph_path);
  if (status.ok()) {
    std::cout << "Succes!" << std::endl;
  } else {
    std::cout << "Failed to run the graph: " << status.message() << std::endl;
    return EXIT_FAILURE;
  }
}
