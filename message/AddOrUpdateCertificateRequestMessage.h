//
// Created by mingj on 2020/5/31.
//

#ifndef MIRCERTIFICATIONMANAGER_ADDORUPDATECERTIFICATEREQUESTMESSAGE_H
#define MIRCERTIFICATIONMANAGER_ADDORUPDATECERTIFICATEREQUESTMESSAGE_H

#include "BaseRequestMessage.h"


class AddOrUpdateCertificateRequestMessage : public BaseRequestMessage {
public:
    struct AddOrUpdateCertificateRequestData {
        std::string certStr = "";
        long lifetime = -1;
        bool forceUpdate = false;
    };

    std::string getErrorMessage(int responseCode) override {
        switch (responseCode) {
            case 0:
                return "";
            case -1:
                return "证书无效";
            case -2:
                return "存在同名证书，且未覆盖";
            default:
                return "未知错误";
        }
    }

    BaseRequestMessage *parse(const std::string &jsonStr) override {
        Json::Reader reader;
        Json::Value root;
        reader.parse(jsonStr, root);

        this->code = code;
        int code = root["code"].asInt();
        if (code != 1)
            return this;
        addOrUpdateCertificateRequestData.certStr = root["data"]["certStr"].asString();
        addOrUpdateCertificateRequestData.lifetime = root["data"]["lifetime"].asUInt64();
        addOrUpdateCertificateRequestData.forceUpdate = root["data"]["forceUpdate"].asBool();
        return this;
    }


    std::string getCertStr() const {
        return addOrUpdateCertificateRequestData.certStr;
    }

    long getLifetime() const {
        return addOrUpdateCertificateRequestData.lifetime;
    }

    bool isForceUpdate() const {
        return addOrUpdateCertificateRequestData.forceUpdate;
    }

private:
    AddOrUpdateCertificateRequestData addOrUpdateCertificateRequestData;
};


#endif //MIRCERTIFICATIONMANAGER_ADDORUPDATECERTIFICATEREQUESTMESSAGE_H
