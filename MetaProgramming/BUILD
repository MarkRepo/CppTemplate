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

cc_test(
    name = "variadic-test",
    srcs = ["variadic_tpl_test.cc","variadic_tpl.h", "meta.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)

cc_test(
    name = "unit-test",
    srcs = ["unit_test.cc","unit.h", "meta.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)
