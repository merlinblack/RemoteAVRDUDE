cmake_minimum_required(VERSION 3.0.0)
project(RemoteAVRDUDE VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(RemoteAVRDUDE main.cpp config.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
