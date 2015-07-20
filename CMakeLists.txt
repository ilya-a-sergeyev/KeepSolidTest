cmake_minimum_required(VERSION 3.2)
project(KeepSolidTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenSSL REQUIRED)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR})

set (KeepSolidTestProtoSources
    pbs/message_common.proto
    pbs/message.proto
    pbs/message_type.proto
    pbs/message_workgroup.proto
)

set(SOURCE_FILES main.cpp todo_post_auth.cpp todo_ssl_transport.cpp todo_checklist.cpp)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${KeepSolidTestProtoSources})

add_executable(KeepSolidTest ${KeepSolidTestProtoSources} ${SOURCE_FILES} ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(KeepSolidTest
     ${PROTOBUF_LIBRARY}
     ${OPENSSL_LIBRARIES}
     curl
     jsoncpp
)