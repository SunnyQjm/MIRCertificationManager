cmake_minimum_required(VERSION 3.16)
project(MIRCertificationManager)

set(CMAKE_CXX_STANDARD 14)

add_executable(MIRCertificationManager main.cpp
        utils/RedisUtil.cpp
        utils/RedisUtil.cpp
        utils/NDNCertificationUtil.cpp
        utils/JsonCppUtil.cpp
        Server.cpp
        )
target_link_libraries(MIRCertificationManager ndn-cxx boost_system hiredis jsoncpp)

add_executable(installCertificate installCertificate.cpp
        utils/FileUtils.cpp
        utils/RedisUtil.cpp
        utils/NDNCertificationUtil.cpp)
target_link_libraries(installCertificate ndn-cxx boost_system hiredis)

add_executable(uninstallCertificate uninstallCertificate.cpp
        utils/FileUtils.cpp
        utils/RedisUtil.cpp
        utils/NDNCertificationUtil.cpp)
target_link_libraries(uninstallCertificate ndn-cxx boost_system hiredis)
