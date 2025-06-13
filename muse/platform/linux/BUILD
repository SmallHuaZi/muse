load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "headers",
    hdrs = [
        "include/muse/platform/socket.hpp",
        "include/muse/platform/timer.hpp",
    ],
    includes = [
        "include"
    ],
    visibility = [
        "//visibility:public"
    ]
)

cc_library(
    name = "platform",
    srcs = [
        "epoll.cpp"
    ],
    deps = [
        ":headers",
        "//muse/include:core",
        "@spdlog//:spdlog",
    ],
    defines = [
        "MUSE_DEBUG=1"
    ],
    visibility = [
        "//visibility:public"
    ]
)