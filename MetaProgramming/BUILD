cc_test(
    name = "meta-test",
    srcs = ["meta_test.cc","meta.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)

cc_test(
    name = "enable-if-test",
    srcs = ["enable_if_test.cc","enable_if.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)