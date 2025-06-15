load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "runtime",
    srcs = [
        "scheduler.cpp",
        "task.cpp",
    ],
    hdrs = [
        "scheduler.hpp",
        "task.hpp",
    ],
    includes = [
        "../"
    ],
    deps = [
        "//muse/include:core",
        "//muse/lib/util:util",

        # Third party dependencies
        "@boost.intrusive//:boost.intrusive",
        "@spdlog//:spdlog",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

