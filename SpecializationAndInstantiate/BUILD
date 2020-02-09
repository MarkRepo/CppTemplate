load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "extern_tpl",
    srcs = ["extern_tpl.cc"],
    hdrs = ["MyVector.h"],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "specialization-test",
    srcs = ["specialization_test.cc","specialization.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)

cc_test(
    name = "instantiate-test",
    srcs = ["instantiate_test.cc","instantiate.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ":extern_tpl",
            ],
)

