//
// Created by mingj on 2020/6/2.
//

#ifndef MIRCERTIFICATIONMANAGER_EXISTSCERTIFICATEREQUESTMESSAGE_H
#define MIRCERTIFICATIONMANAGER_EXISTSCERTIFICATEREQUESTMESSAGE_H

#include "BaseRequestMessage.h"

class ExistsCertificateRequestMessage : BaseRequestMessage {
public:
    struct ExistsCertificateRequestData {
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

        this->code = code;
        int code = root["code"].asInt();
        if (code != 1)
            return this;
        existsCertificateRequestData.certStr = root["data"]["certStr"].asString();
        return this;
    }

    std::string getCertStr() const {
        return existsCertificateRequestData.certStr;
    }

private:
    ExistsCertificateRequestData existsCertificateRequestData;
};
#endif //MIRCERTIFICATIONMANAGER_EXISTSCERTIFICATEREQUESTMESSAGE_H
