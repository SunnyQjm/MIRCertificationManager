cmake_minimum_required(VERSION 3.16)
project(MIRCertificationManager)

set(CMAKE_CXX_STANDARD 14)

add_executable(MIRCertificationManager main.cpp utils/RedisUtil.cpp)
target_link_libraries(MIRCertificationManager hiredis)