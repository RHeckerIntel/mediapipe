cc_library(
    name = "graph_runner",
    srcs = ["graph_runner.cc"],
    hdrs = ["graph_runner.h"],
    deps = [
        "@model_api//:model_api",
        "//mediapipe/calculators/geti/inference:inference_calculators",
        "//mediapipe/calculators/geti/utils:utils",
        "//mediapipe/calculators/geti/serialization:calculators",
        "//mediapipe/framework/port:file_helpers",
        "//mediapipe/framework:calculator_framework",
        "//mediapipe/framework/port:parse_text_proto",
        "//mediapipe/framework/port:status",
    ],

)

cc_library(
    name = "bindings",
    hdrs = ["bindings.h"],
    srcs = ["bindings.cc"],
    deps = [
        "//mediapipe/framework/port:logging",
        "@com_github_glog_glog//:glog",
        ":graph_runner",
    ],
    copts = ["-fPIC"],  # Ensure position-independent code
    alwayslink=1,
    linkopts = ["-Wl,-rpath,./lib"],
)

cc_binary(
    name = "scratch",
    srcs = [ "scratch.cc",],
    deps = [":bindings"]
)

cc_binary(
    name = "cc_bindings",
    deps = [
        ":bindings",
    ],
    linkshared=1
)
