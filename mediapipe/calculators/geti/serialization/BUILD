load("//mediapipe/framework/port:build_config.bzl", "mediapipe_cc_proto_library", "mediapipe_proto_library")

cc_library(
    name = "serialization",
    hdrs = [
        "result_serialization.h",
        "csv_serialization.h",
        "project_serialization.h",
    ],
    deps = [
        "//mediapipe/calculators/geti/utils:data_structures",
        "//mediapipe/framework/port:opencv_imgcodecs",
        "//third_party/cpp-base64:cpp-base64",
        "@nlohmann_json//:json",
    ],
    visibility = [
        "//visibility:public",
    ],
)


filegroup(
    name = "test_data",
    srcs = glob(["*.json"]),
)

cc_library(
    name = "calculators",
    hdrs = [
        "serialization_calculators.h",
        "app_serialization_calculator.h",

    ],
    srcs = [
        "serialization_calculators.cc",
        "app_serialization_calculator.cc",

    ],
    deps = [
        "//mediapipe/calculators/geti/inference:inference_calculators",
        "//mediapipe/calculators/geti/inference:geti_calculator_base",
        "//mediapipe/calculators/geti/inference:inference_utils",
        ":serialization",
        ":app_serialization_calculator_cc_proto",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink=1
)

proto_library(
    name = "app_serialization_calculator_proto",
    srcs = ["app_serialization_calculator.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "//mediapipe/framework:calculator_proto",

    ],
)

mediapipe_cc_proto_library(
    name = "app_serialization_calculator_cc_proto",
    srcs = ["app_serialization_calculator.proto"],
    cc_deps = ["//mediapipe/framework:calculator_cc_proto"],
    visibility = ["//visibility:public"],
    deps = [":overlay_calculator_proto"],
)

cc_test(
    name = "serialization_calculators_test",
    srcs = ["serialization_calculators_test.cc" ],
    data = [
        ":test_data"
    ],
    deps = [
        ":calculators",
        "//mediapipe/calculators/geti/inference:test_deps"
    ]
)


