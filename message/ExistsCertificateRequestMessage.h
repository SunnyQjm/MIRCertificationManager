//
// Created by mingj on 2020/6/2.
//

#ifndef MIRCERTIFICATIONMANAGER_EXISTSCERTIFICATEREQUESTMESSAGE_H
#define MIRCERTIFICATIONMANAGER_EXISTSCERTIFICATEREQUESTMESSAGE_H

#include "BaseRequestMessage.h"

class ExistsCertificateRequestMessage : public BaseRequestMessage {
public:
    struct ExistsCertificateRequestData {
        std::string prefix = "";        //  用户前缀
        std::string certStr = "";
    };

    std::string getErrorMessage(int responseCode) override {
        switch (responseCode) {
            case 0:
                return "";
            case -1:
                return "没有对应的条目";
            default:
                return "未知错误";
        }
    }

    BaseRequestMessage * parse(const std::string &jsonStr) override {
        Json::Reader reader;
        Json::Value root;
        reader.parse(jsonStr, root);

        this->code = root["Code"].asInt();
        this->timestamp = root["Timestamp"].asString();
        existsCertificateRequestData.prefix = root["Data"]["Prefix"].asString();
        existsCertificateRequestData.certStr = root["Data"]["CertStr"].asString();
        return this;
    }

    std::string getCertStr() const {
        return existsCertificateRequestData.certStr;
    }

    std::string getPrefix() const {
        return existsCertificateRequestData.prefix;
    }

private:
    ExistsCertificateRequestData existsCertificateRequestData;
};
#endif //MIRCERTIFICATIONMANAGER_EXISTSCERTIFICATEREQUESTMESSAGE_H
