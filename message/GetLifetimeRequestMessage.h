//
// Created by mingj on 2020/6/2.
//

#ifndef MIRCERTIFICATIONMANAGER_GETLIFETIMEREQUESTMESSAGE_H
#define MIRCERTIFICATIONMANAGER_GETLIFETIMEREQUESTMESSAGE_H

#include "BaseRequestMessage.h"

class GetLifetimeRequestMessage : BaseRequestMessage {
public:
    struct GetLifetimeRequestData {
        std::string certStr = "";
    };

    std::string getErrorMessage(int responseCode) override {
        if (responseCode > 0) {
            return "";
        }
        switch (responseCode) {
            case 0:
                return "";
            case -2:
                return "没有对应的条目";
            default:
                return "未知错误";
        }
    }

    BaseRequestMessage *parse(const std::string &jsonStr) override {
        Json::Reader reader;
        Json::Value root;
        reader.parse(jsonStr, root);

        this->code = code;
        getLifetimeRequestData.certStr = root["data"]["certStr"].asString();
        return this;
    }

    std::string getCertStr() const {
        return getLifetimeRequestData.certStr;
    }

private:
    GetLifetimeRequestData getLifetimeRequestData;
};

#endif //MIRCERTIFICATIONMANAGER_GETLIFETIMEREQUESTMESSAGE_H