//
// Created by mingj on 2020/5/31.
//

#ifndef MIRCERTIFICATIONMANAGER_SERVER_H
#define MIRCERTIFICATIONMANAGER_SERVER_H

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <string>
#include "utils/NDNCertificationUtil.h"

using namespace ndn;

class Server {
public:
    explicit
    Server(std::string prefix = "/localmanager/mircertification");

    void run();

private:
    void onInterest(const InterestFilter &filter, const Interest &interest);

    void onRegisterFailed(const Name &prefix, const std::string &reason);

private:
    Face face;
    KeyChain keyChain;
    NDNCertificationUtil ndnCertificationUtil;
    std::string registerPrefix;
};


#endif //MIRCERTIFICATIONMANAGER_SERVER_H
