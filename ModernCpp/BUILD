cc_test(
    name = "techniques-test",
    srcs = ["techniques.cc","techniques.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)