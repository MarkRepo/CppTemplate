load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "typeinfodemangle",
    srcs = ["typeinfodemangle.cc"],
    hdrs = ["typeinfodemangle.h"],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "demangle-test",
    srcs = ["typeinfodemangle_test.cc", "typeinfodemangle.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
            ],
    deps = ["@gtest//:main",
            ":typeinfodemangle",
            ],
)
