cmake_minimum_required(VERSION 3.0.0)
project(RemoteAVRDUDE VERSION 0.1.0)

add_executable(RemoteAVRDUDE main.cpp configuration.cpp externalcall.cpp stringmanip.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "ravrdude")
#target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
