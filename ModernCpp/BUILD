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

cc_test(
    name = "techniques2-test",
    srcs = ["techniques2.cc","techniques2.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)

cc_test(
    name = "typelists-test",
    srcs = ["typelists.cc","typelists.h", "techniques2.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)

cc_test(
    name = "varargslists-test",
    srcs = ["typelists_varargs.cc","typelists_varargs.h", "techniques2.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)

cc_test(
    name = "smallobjallocation-test",
    srcs = ["smallobjallocation.cc","smallobjallocation.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)

cc_test(
    name = "singleton-test",
    srcs = ["singleton.cc","singleton.h"],
    copts = ["-Iexternal/gtest",
             "-Iexternal/gtest/include",
             ],
    deps = ["@gtest//:main",
            "//CommonTools:typeinfodemangle",
            ],
)