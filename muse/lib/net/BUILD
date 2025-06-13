cc_library(
    name = "headers",
    hdrs = [
        "include/net/ip_addr.hpp",
        "include/net/socket_addr.hpp",
    ],
    includes = ["include"],
    deps = [
        "//muse/include:base"
    ],
    visibility = [
        "//visibility:public"
    ]
)

cc_library(
    name = "net",
    srcs = [
        "ip_addr.cpp"
    ],
    deps = [
        ":headers",
    ],
    visibility = [
        "//visibility:public"
    ]
)