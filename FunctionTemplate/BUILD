cc_test(
    name = "functpl-test",
    srcs = ["function_test.cc","function.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)