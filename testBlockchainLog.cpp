//
// Created by sunny on 2020/9/9.
//

#include <iostream>
#include "utils/RingBuffer.h"
#include "utils/json.hpp"

int main() {
  RingBuffer ringBuffer;
  ringBuffer.open("MIN-MIR-WARNING-LOG-TO-BLOCKCHAIN");
  nlohmann::json logJsonInfo = {
      {"Type", "network"},
      {"Command", "Log"},
      {"Prefix", "xxx"},
      {"Level", 1},
      {"Action", R"({"Action":"","Weight":90,"User":"dmgo--","GateIn":"/min/gdcni16","VRouter":"/min/gdcni11"})"},
      {"Sig", "xxx"},
      {"Timestamp", "xxx"}
  };
  std::string logInfo = logJsonInfo.dump();
  int size = logInfo.size();
  ringBuffer.write((void *) (&size), 4);
  ringBuffer.write((void *) logInfo.c_str(), logInfo.size());
  sleep(5);
  ringBuffer.write((void *) (&size), 4);
  ringBuffer.write((void *) logInfo.c_str(), logInfo.size() - 2);
  sleep(2);
  ringBuffer.write((void *) (logInfo.c_str() + logInfo.size() - 2), 2);
  sleep(5);
  ringBuffer.write((void *) (&size), 4);
  ringBuffer.write((void *) logInfo.c_str(), logInfo.size());
  sleep(5);
  ringBuffer.write((void *) (&size), 4);
  ringBuffer.write((void *) logInfo.c_str(), logInfo.size());
  sleep(5);
  ringBuffer.write((void *) (&size), 4);
  ringBuffer.write((void *) logInfo.c_str(), logInfo.size());
  sleep(5);
  ringBuffer.write((void *) (&size), 4);
  ringBuffer.write((void *) logInfo.c_str(), logInfo.size());
  sleep(5);
  ringBuffer.remove();
}
