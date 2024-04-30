#include "bindings.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <thread>

void handle_input(const char* message) {
    std::cout << "got output message" << std::endl;
}

void listen_task(void* instance) {
    GraphRunner_Listen(instance, handle_input);
}

static const char* graph = R"pb(
 input_stream : "input"
          output_stream : "output"


          node {
          calculator : "OpenVINOInferenceAdapterCalculator"
          output_side_packet : "INFERENCE_ADAPTER:adapter_0"
          node_options: {
            [type.googleapis.com/mediapipe.OpenVINOInferenceAdapterCalculatorOptions] {
                model_path: "/data/geti/detection_ssd.xml"
            }
          }
          }

        node {
          calculator : "DetectionCalculator"
          input_side_packet : "INFERENCE_ADAPTER:adapter_0"
          input_stream : "IMAGE:input"
          output_stream: "INFERENCE_RESULT:result"
        }

        node {
          calculator : "OverlayCalculator"
          input_stream : "IMAGE:input"
          input_stream : "INFERENCE_RESULT:result"
          output_stream : "IMAGE:output"
        }
        )pb";

int main() {
    std::cout << "scratch" << std::endl;

    //auto image = cv::imread(image_path);
    //std::cout << image.size() << std::endl;

    //// yuck...
    //std::vector<uchar> buf;
    //cv::imencode(".jpg", image, buf);
    //std::vector<char> image_data(buf.begin(), buf.end());


    //SerializeModel("C:/Users/selse/AppData/Roaming/intel.geti/inference/65c49dc49467d132a02da500/model.xml", "classification", "C:/Users/selse/AppData/Roaming/intel.geti/inference/65c49dc49467d132a02da500/serialized.xml");

    SerializeModel(
        "/home/rhecker/.local/share/com.example.inference/662baad4ac9c95a4518c0aaa/662baad8ac9c95a4518c0aae_model.xml",
        "instance_segmentation",
        "/home/rhecker/.local/share/com.example.inference/662baad4ac9c95a4518c0aaa/662baad8ac9c95a4518c0aae.xml"
        );

    {
        std::ifstream image_file("/data/cattle.jpg", std::ifstream::binary);
        ////std::vector<char> image_data((std::istreambuf_iterator<char>(image_file)), std::istreambuf_iterator<char>());
        std::stringstream ss;
        ss << image_file.rdbuf();

        std::string data = ss.str();
        std::string result = GraphRunner_Run(graph, data.c_str(), data.size());
    }

    //std::cout << "graph open style: " << std::endl;
    //auto instance = GraphRunner_Open("detection", "C:/data/detection_ssd.xml");
    //std::thread t1(listen_task, instance);
    //for ( int i = 0; i < 10; i++ ) {
    //    GraphRunner_Queue(instance, "C:/data/cattle.jpg");
    //}

    //std::this_thread::sleep_for(std::chrono::seconds(1));

    //GraphRunner_Queue(instance, "C:/data/cattle.jpg");


    //GraphRunner_Close(instance);

    //t1.join();
    return 0;
}
