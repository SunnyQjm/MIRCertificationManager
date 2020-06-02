//
// Created by mingj on 2020/6/2.
//

#ifndef MIRCERTIFICATIONMANAGER_REMOVECERTIFICATEREQUESTMESSAGE_H
#define MIRCERTIFICATIONMANAGER_REMOVECERTIFICATEREQUESTMESSAGE_H

#include "BaseRequestMessage.h"

class RemoveCertificateRequestMessage : BaseRequestMessage {
public:
    struct RemoveCertificateRequestData {
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

        this->code = code;
        int code = root["code"].asInt();
        if (code != 1)
            return this;
        removeCertificateRequestData.certStr = root["data"]["certStr"].asString();
        return this;
    }

    std::string getCertStr() const {
        return removeCertificateRequestData.certStr;
    }

private:
    RemoveCertificateRequestData removeCertificateRequestData;
};

#endif //MIRCERTIFICATIONMANAGER_REMOVECERTIFICATEREQUESTMESSAGE_H
