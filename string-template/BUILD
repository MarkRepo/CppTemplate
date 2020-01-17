cc_library(
    name = "string-template",
    srcs = ["String.cc"],
    hdrs = ["String.h"],
)

cc_test(
    name = "string-test",
    srcs = ["String.test.cc"],
    copts = ["-Iexternal/gtest/include/",
	     "-Iexternal/gtest",
	    ],
    deps = [
        "@gtest//:main",
        "//:string-template",
    ],
)