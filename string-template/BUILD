load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "string-template",
    srcs = ["String.cc"],
    hdrs = ["String.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "string-test",
    srcs = ["String_test.cc"],
    copts = ["-Iexternal/gtest/include/",
	     "-Iexternal/gtest",
	    ],
    deps = [
        "@gtest//:main",
        ":string-template",
    ],
)
