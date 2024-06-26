load("@rules_foreign_cc//foreign_cc:cmake.bzl", "cmake")
visibility = ["//visibility:public"]

filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)

cmake(
    name = "blend2d_cmake",
    build_args = [
        "--verbose",
        "--",  # <- Pass remaining options to the native tool.
        # https://github.com/bazelbuild/rules_foreign_cc/issues/329
        # there is no elegant paralell compilation support
        "VERBOSE=1",
        "-j 4",
    ],
    cache_entries = {
        "CMAKE_POSITION_INDEPENDENT_CODE": "ON",
    },
    lib_source = ":all_srcs",
    out_shared_libs = ["libblend2d.so"],
    build_data = [
        "@asmjit//:all_srcs"
    ]
)

cc_library(
    name = "blend2d",
    deps = [
        ":blend2d_cmake",
    ],
    visibility = ["//visibility:public"],
)
