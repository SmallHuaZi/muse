load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "base",
    hdrs = [
        "muse/types.hpp",
        "muse/status.hpp",
        "muse/assert.hpp",
    ],
    includes = ["."],
    defines = [
        "MUSE_DEBUG=1"
    ],
    visibility = [
        "//visibility:public"
    ]
)

cc_library(
    name = "core",
    hdrs = [
        "muse/log.hpp",
        "muse/acceptor.hpp",
        "muse/channel.hpp",
        "muse/event.hpp",
        "muse/event_handler.hpp",
        "muse/event_loop.hpp",
        "muse/event_loop_thread.hpp",
        "muse/poller.hpp",
        "muse/work-queue.hpp",
        "muse/timer-queue.hpp",
        "muse/tcp-connection.hpp",
        "muse/tcp-server.hpp",

        "muse/base/socket.hpp",
        "muse/poller/epoll.hpp",
    ],
    includes = [
        "."
    ],
    defines = [
        "MUSE_DEBUG=1"
    ],
    deps = [
        ":base",
        "//muse/lib/util:util",
        "//muse/lib/enumbits:enumbits",
        "//muse/lib/net:headers"
    ],
    visibility = [
        "//visibility:public"
    ]
)