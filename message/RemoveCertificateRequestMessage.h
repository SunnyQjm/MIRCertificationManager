//
// Created by mingj on 2020/6/2.
//

#ifndef MIRCERTIFICATIONMANAGER_REMOVECERTIFICATEREQUESTMESSAGE_H
#define MIRCERTIFICATIONMANAGER_REMOVECERTIFICATEREQUESTMESSAGE_H

#include "BaseRequestMessage.h"

class RemoveCertificateRequestMessage : public BaseRequestMessage {
public:
    struct RemoveCertificateRequestData {
        std::string prefix = "";        //  用户前缀
        std::string certStr = "";
    };

    std::string getErrorMessage(int responseCode) override {
        switch (responseCode) {
            case 0:
                return "";
            case -1:
                return "证书无效";
            case -2:
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
        removeCertificateRequestData.prefix = root["Data"]["Prefix"].asString();
        removeCertificateRequestData.certStr = root["Data"]["CertStr"].asString();
        return this;
    }

    std::string getCertStr() const {
        return removeCertificateRequestData.certStr;
    }

    std::string getPrefix() const {
        return removeCertificateRequestData.prefix;
    }
private:
    RemoveCertificateRequestData removeCertificateRequestData;
};

#endif //MIRCERTIFICATIONMANAGER_REMOVECERTIFICATEREQUESTMESSAGE_H
