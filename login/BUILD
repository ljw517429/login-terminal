# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
    name = "server",
    srcs = ["login_server.cc"],
    deps = [
        "//util:util",
        "//cache:cache",
        "//model:model",
        "//hiredis:hiredis",
        "//protos:loginpb",
    ],
    linkstatic = 0,
)

cc_binary(
    name = "client",
    srcs = ["login_client.cc"],
    deps = [
        "//util:util",
        "//cache:cache",
        "//model:model",
        "//hiredis:hiredis",
        "//protos:loginpb",
    ],
    linkstatic = 0,
)

# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_library
# cc_library(
#     name = "rule unique name",
#     srcs = ["source files"],
#     hdrs = ["header files"],
#     deps = ["header files"],
# )