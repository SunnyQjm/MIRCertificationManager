//
// Created by sunny on 2020/9/9.
//

#ifndef MIRCERTIFICATIONMANAGER_BLOCKCHAINLOGGER_H
#define MIRCERTIFICATIONMANAGER_BLOCKCHAINLOGGER_H

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>

using namespace ndn;

/**
 * 负责从NFD接收日志信息，并发送给区块链
 */
class BlockChainLogger {
public:
  /**
   * 创建区块链日志记录对象
   * @param host            区块链节点所接入的NFD的ip（如果本地NFD有路由可以转发到区块链所接入的节点，直接连接本地的NFD也可）
   * @param port            区块链节点所接入的NFD的port（如果本地NFD有路由可以转发到区块链所接入的节点，直接连接本地的NFD也可）
   * @param prefix          区块链监听在这个前缀接收日志信息
   */
  explicit
  BlockChainLogger(std::string shareMemoryName = "MIN-MIR-WARNING-LOG-TO-BLOCKCHAIN",
                   std::string host = "localhost", int port = 6363,
                   std::string prefix = "/mis1");

  /**
   * 运行日志转发服务
   */
  void run();

private:


private:
  Face face;
  KeyChain keyChain;
  std::string prefix;
  std::string host;
  std::string shareMemoryName;
  int port;
};


#endif //MIRCERTIFICATIONMANAGER_BLOCKCHAINLOGGER_H
