cmake_minimum_required(VERSION 3.5)
project(MIRCertificationManager)

set(CMAKE_CXX_STANDARD 14)

add_executable(MIRCertificationManager main.cpp
        utils/RedisUtil.cpp
        utils/RedisUtil.cpp
        utils/NDNCertificationUtil.cpp
        utils/JsonCppUtil.cpp
        utils/RingBuffer.cpp
        Server.cpp
        BlockChainLogger.cpp BlockChainLogger.h)
target_link_libraries(MIRCertificationManager ndn-cxx boost_system hiredis jsoncpp boost_thread pthread rt)

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

add_executable(testRequest testRequest.cpp
        utils/FileUtils.cpp
        )

target_link_libraries(testRequest ndn-cxx boost_system)

add_executable(testBlockchainLog testBlockchainLog.cpp
        utils/RingBuffer.cpp
        )

target_link_libraries(testBlockchainLog ndn-cxx boost_system hiredis jsoncpp boost_thread pthread rt)
