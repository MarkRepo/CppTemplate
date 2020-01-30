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