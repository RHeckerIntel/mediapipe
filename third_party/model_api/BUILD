licenses(["notice"])  # Apache License 2.0

alias(
    name = "model_api",
    actual = select({
        "//mediapipe:windows": "@model_api//:model_api_windows",
        "//conditions:default": "@model_api//:model_api_linux",
    }),
    visibility = ["//visibility:public"],
)
