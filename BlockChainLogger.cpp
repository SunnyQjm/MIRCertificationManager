//
// Created by sunny on 2020/9/9.
//
#include <utility>
#include "BlockChainLogger.h"
#include "utils/RingBuffer.h"
#include <ndn-cxx/lp/cache-policy.hpp>
#include <ndn-cxx/lp/tags.hpp>


BlockChainLogger::BlockChainLogger(std::string shareMemoryName,
                                   std::string host, int port, std::string prefix) :
    host(std::move(host)), port(port), prefix(std::move(prefix)), face(host, std::to_string(port)),
    shareMemoryName(std::move(shareMemoryName)) {
}

void BlockChainLogger::run() {
  RingBuffer ringBuffer;

  if (!ringBuffer.connect(this->shareMemoryName)) {
    // 打开共享内存失败，可能是NFD没有启动
    std::cerr << "连接共享内存: " << shareMemoryName << " => 失败~" << std::endl;
    exit(-1);
  }
  char buffer[4 * 1024 * 1024];
  int expectedSize = 0;
  int size = 0;
  int offset = 0;

  while (true) {
    if (expectedSize <= 0) {       // 读取一个整型，表示后面跟着的json串
      size = ringBuffer.read(buffer, 4);
      if (size <= 0) {    // 没有数据
//        std::cout << "wait for data" << std::endl;
        usleep(10000);
        continue;
      }
      expectedSize = *((int *) buffer);
      offset = 0;
    }
    size = ringBuffer.read(buffer + offset, expectedSize);
    expectedSize -= size;
    offset += size;
    if (expectedSize == 0) {   // 读取到完整的一个json串
      std::string logInfo(buffer, offset);
      std::cout << logInfo << std::endl;

      // 通过MIN的方式，将日志信息发给区块链
      Interest interest((Name(prefix)));
      interest.setMustBeFresh(true);
      Data data;
      ndn::lp::CachePolicy cachePolicy;
      cachePolicy.setPolicy(ndn::lp::CachePolicyType::NO_CACHE);
      data.setTag(make_shared<ndn::lp::CachePolicyTag>(cachePolicy));
      interest.setApplicationParameters((const uint8_t *) buffer, offset);
      keyChain.sign(interest);
      face.expressInterest(interest, [](const Interest &interest1, const Data &data) {
        // onData
        std::cout << "onData" << std::endl;
      }, [](const Interest &interest1, const lp::Nack &nack) {
        // onNack
        std::cout << "onNack: " << nack.getReason() << std::endl;
      }, [](const Interest &interest1) {
        // onTimeout
        std::cout << "onTimeout" << std::endl;
      });
      face.processEvents();
    } else {
//      std::cout << "数据接收不完整" << std::endl;
    }
  }
}
