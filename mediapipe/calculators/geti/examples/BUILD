licenses(["notice"])

package(default_visibility = ["//visibility:public"])
load("@mediapipe//mediapipe/framework/tool:mediapipe_graph.bzl", "mediapipe_simple_subgraph")

cc_library(
    name = "demo_dependencies",
    deps = [
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/formats:image",
        "//mediapipe/framework/formats:image_frame",
        "//mediapipe/framework/formats:image_frame_opencv",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework/port:opencv_highgui",
        "//mediapipe/framework/port:opencv_imgproc",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "//mediapipe/calculators/geti/inference:inference_calculators",
        "//mediapipe/calculators/geti/utils:utils",
        "//mediapipe/calculators/geti/serialization:calculators"
    ]
)

cc_library(
    name = "test_calculator",
    srcs = ["test_calculator.cc"],
    hdrs = ["test_calculator.h"],
    deps = [
        "//mediapipe/framework:calculator_framework",
    ],
    alwayslink=1

)

cc_binary(
    name = "calculator_demo",
    srcs = ["calculator_demo.cc"],
    data = [
        "//mediapipe/calculators/geti/graphs:graphs",
    ],
    deps = [
        ":demo_dependencies",
    ],
)

cc_binary(
    name = "llm_demo",
    srcs = ["llm_demo.cc"],
    data = [
        "//mediapipe/calculators/geti/graphs:graphs",
    ],
    deps = [
        ":demo_dependencies",
        ":test_calculator",
    ],
)

cc_binary(
    name = "scratch",
    srcs = ["scratch.cc"],
    data = [
        "//mediapipe/calculators/geti/graphs:graphs",
    ],
    deps = [
        "//mediapipe/calculators/geti/utils:overlay",
        "//mediapipe/calculators/geti/serialization:serialization",
    ],
)
