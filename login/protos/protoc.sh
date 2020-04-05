#!/bin/bash

build_proto_v3() {
    cwd=`pwd`

    protoc \
    -I. \
    --grpc_out="${cwd}" \
    --plugin="protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin" \
    --cpp_out="${cwd}" \
    ./*.proto
}

build_proto_v3