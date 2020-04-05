cc_library(
    name = "loginpb",
    srcs = [
        'login.grpc.pb.cc', 
        'login.pb.cc', 
    ],
    hdrs = [
        'login.grpc.pb.h',
        'login.pb.h',
    ],
    deps = [
        '@com_google_protobuf//:protobuf',
        '@com_github_grpc_grpc//:grpc++',
    ],
    visibility = ["//visibility:public"],
)