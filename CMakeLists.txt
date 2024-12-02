cmake_minimum_required(VERSION 3.21)

project(gRPC-Chat-Server C CXX)
set(CMAKE_CXX_STANDARD 17)

include(cmake/find_grpc.cmake)
include(cmake/compile_grpc_proto.cmake)

compile_grpc_proto(grpc_proto_chat api_chat "src/api_chat.proto")

add_executable(chat_api_server
    src/server.cpp
    src/authenticator.cpp
    src/chat_api_login.cpp
    src/chat_api_message_fetch.cpp
    src/chat_api_message_send.cpp
    src/simple_chat_api_message_fetch.cpp
    src/simple_chat_api_message_send.cpp
    )

target_link_libraries(
    chat_api_server
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    grpc_proto_chat
)