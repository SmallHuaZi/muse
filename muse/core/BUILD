load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "core",
    srcs = [
        "acceptor.cpp",
        "channel.cpp",
        "event_loop.cpp",
        "event_loop_thread.cpp",
        "poller.cpp",
        "tcp-connection.cpp",
        "work-queue.cpp",
    ],
    deps = [
        "//muse/include:core",
        "//muse/lib/util:util",

        # Third party dependencies
        "@boost.intrusive//:boost.intrusive",
        "@spdlog//:spdlog",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ] + select({
        ":is_on_windows": ["//muse/platform/windows:platform"],
        ":is_on_linux": ["//muse/platform/linux:platform"],
    }),
)

config_setting(
    name = "is_on_windows",
    constraint_values = [
        "@platforms//os:windows",
    ],
)

config_setting(
    name = "is_on_linux",
    constraint_values = [
        "@platforms//os:linux",
    ],
)