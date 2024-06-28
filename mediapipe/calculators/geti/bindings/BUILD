load("//mediapipe/calculators/geti/bindings:windows_dll_library.bzl", "windows_dll_library")
licenses(["notice"])

package(default_visibility = ["//visibility:public"])

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
    name = "llm_inference",
    srcs = ["llm_inference.cc"],
    hdrs = ["llm_inference.h"],
    deps = [
        "@linux_openvino//:openvino",
    ]
)

cc_library(
    name = "bindings",
    hdrs = ["bindings.h"],
    srcs = ["bindings.cc"],
    deps = [
        "//mediapipe/framework/port:logging",
        "@com_github_glog_glog//:glog",
        ":graph_runner",
        ":llm_inference",
    ],
    copts = ["-fPIC"],  # Ensure position-independent code
    alwayslink=1,
    linkopts = ["-Wl,-rpath,$$ORIGIN/lib"],
)

windows_dll_library(
    name = "windows_bindings",
    srcs = [ "bindings.cc" ],
    hdrs = [ "bindings.h" ],
    deps = [
        "@windows_pugixml//:pugixml",
        "@windows_tbb//:tbb",
        ":graph_runner"
    ],
    copts = ["/DCOMPILING_DLL"],
)

cc_binary(
    name = "scratch",
    srcs = [ "scratch.cc",],
    deps = [":bindings"]
)

cc_binary(
    name = "scratch2",
    srcs = [ "scratch2.cc",],
    deps = [":bindings"]
)

cc_binary(
    name = "linux_bindings",
    deps = [
        ":bindings",
    ],
    linkshared=1
)
