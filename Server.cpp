//
// Created by mingj on 2020/5/31.
//

#include "Server.h"
#include <utility>
#include "utils/JsonCppUtil.h"
#include "message/BaseRequestMessage.h"
#include "message/AddOrUpdateCertificateRequestMessage.h"
#include "message/RemoveCertificateRequestMessage.h"
#include "message/GetLifetimeRequestMessage.h"
#include "message/ExistsCertificateRequestMessage.h"
#include <ndn-cxx/lp/cache-policy.hpp>
#include <ndn-cxx/lp/tags.hpp>


Server::Server(std::string prefix) :
        registerPrefix(std::move(prefix)) {

}

void Server::run() {
    // 连接到本地的redis服务
    ndnCertificationUtil.connect();
    face.setInterestFilter(this->registerPrefix,
                           [this](const InterestFilter &filter, const Interest &interest) {
                               this->onInterest(filter, interest);
                           },
                           RegisterPrefixSuccessCallback(),
                           [this](const Name &prefix, const std::string &reason) {
                               this->onRegisterFailed(prefix, reason);
                           }
    );
    face.processEvents();
}

void Server::onInterest(const InterestFilter &filter, const Interest &interest) {
    std::cout << "onInterest: " << interest.toUri() << std::endl;
    if (interest.getApplicationParameters().value_size() <= 0) {
        std::cerr << "have no application parameters" << std::endl;
        return;
    }
    std::string params((char *) (interest.getApplicationParameters().value()),
                       interest.getApplicationParameters().value_size());
    std::cout << "params: " << params << std::endl;
    JsonCppUtil jsonCppUtil(params);
    Data data(interest.getName());
    Json::Value root;
    int resCode;
    long longResult;
    AddOrUpdateCertificateRequestMessage addOrUpdateCertificateRequestMessage;
    RemoveCertificateRequestMessage removeCertificateRequestMessage;
    GetLifetimeRequestMessage getLifetimeRequestMessage;
    ExistsCertificateRequestMessage existsCertificateRequestMessage;
    switch (jsonCppUtil.getInt("Code")) {
        case BaseRequestMessage::ADD_OR_UPDATE_CERTIFICATE:             // 下发或更新证书
            addOrUpdateCertificateRequestMessage.parse(params);
            root["Prefix"] = addOrUpdateCertificateRequestMessage.getPrefix();
            root["Code"] = addOrUpdateCertificateRequestMessage.getCode();
            root["StatusCode"] = ndnCertificationUtil.installCert(addOrUpdateCertificateRequestMessage.getCertStr(),
                                                                  addOrUpdateCertificateRequestMessage.getLifetime(),
                                                                  addOrUpdateCertificateRequestMessage.isForceUpdate());
            root["ErrMsg"] = addOrUpdateCertificateRequestMessage.getErrorMessage(root["StatusCode"].asInt());
            break;
        case BaseRequestMessage::REMOVE_CERTIFICATE:                    // 撤销证书
            removeCertificateRequestMessage.parse(params);
            resCode = ndnCertificationUtil.uninstallCert(removeCertificateRequestMessage.getCertStr());
            std::cout << removeCertificateRequestMessage.getCertStr() << std::endl;
            if (resCode >= 0) {
                resCode = 0;
            } else {
                resCode = resCode;
            }
            root["Prefix"] = removeCertificateRequestMessage.getPrefix();
            root["Code"] = removeCertificateRequestMessage.getCode();
            root["StatusCode"] = resCode;
            root["ErrMsg"] = removeCertificateRequestMessage.getErrorMessage(resCode);
            break;
        case BaseRequestMessage::GET_CERTIFICATE_LIFETIME:              // 获取证书的生存期
            getLifetimeRequestMessage.parse(params);
            longResult = ndnCertificationUtil.getCertLifetime(getLifetimeRequestMessage.getCertStr());
            std::cout << "longResult: " << longResult << std::endl;
            root["Data"] = (long long int) longResult;
            if (longResult >= 0) {
                resCode = 0;
            } else {
                resCode = (int) longResult;
            }
            root["Prefix"] = getLifetimeRequestMessage.getPrefix();
            root["Code"] = getLifetimeRequestMessage.getCode();
            root["StatusCode"] = resCode;
            root["ErrMsg"] = getLifetimeRequestMessage.getErrorMessage(resCode);
            break;
        case BaseRequestMessage::EXISTS_CERTIFICATE:                    // 证书是否存在
            existsCertificateRequestMessage.parse(params);
            if (ndnCertificationUtil.exists(existsCertificateRequestMessage.getCertStr())) {
                root["StatusCode"] = 0;
            } else {
                root["StatusCode"] = -1;
            }
            root["Code"] = existsCertificateRequestMessage.getCode();
            root["ErrMsg"] = existsCertificateRequestMessage.getErrorMessage(root["StatusCode"].asInt());
            break;
        default:
            std::cerr << "未能处理的请求：" << endl << params << endl;
            break;
    }

    std::cout << "response: " << std::endl << root.toStyledString() << std::endl;
    std::string result = root.toStyledString();
    data.setContent((uint8_t *) result.c_str(), result.size());
    data.setFreshnessPeriod(1_ms);

    // 设置data包不缓存
    lp::CachePolicy cachePolicy;
    cachePolicy.setPolicy(lp::CachePolicyType::NO_CACHE);
    data.setTag(make_shared<lp::CachePolicyTag>(cachePolicy));
    keyChain.sign(data);
    face.put(data);
}

void Server::onRegisterFailed(const Name &prefix, const std::string &reason) {

}

