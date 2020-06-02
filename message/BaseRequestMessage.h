//
// Created by mingj on 2020/5/31.
//

#ifndef MIRCERTIFICATIONMANAGER_BASEREQUESTMESSAGE_H
#define MIRCERTIFICATIONMANAGER_BASEREQUESTMESSAGE_H

#include <string>
#include <utility>
#include <jsoncpp/json/json.h>

class BaseRequestMessage {
public:

    static const int ADD_OR_UPDATE_CERTIFICATE;
    static const int REMOVE_CERTIFICATE;
    static const int GET_CERTIFICATE_LIFETIME;
    static const int EXISTS_CERTIFICATE;

    int getCode() const {
        return code;
    }

    virtual BaseRequestMessage *parse(const std::string &jsonStr) {};

    virtual std::string getErrorMessage(int responseCode) {};

//    static BaseRequestMessage parse(const std::string &jsonStr) {
//        Json::Reader reader;
//        Json::Value root;
//        reader.parse(jsonStr, root);
//
//        int code = root["code"].asInt();
//        switch (code) {
//            case 1:
//                AddOrUpdateCertificateRequestData addOrUpdateCertificateRequestData;
//                addOrUpdateCertificateRequestData.certStr = root["data"]["certStr"].asString();
//                addOrUpdateCertificateRequestData.lifetime = root["data"]["lifetime"].asUInt64();
//                addOrUpdateCertificateRequestData.forceUpdate = root["data"]["forceUpdate"].asBool();
//                return BaseRequestMessage<AddOrUpdateCertificateRequestData>(code, AddOrUpdateCertificateRequestData());
//            case 2:
//                RemoveCertificateRequestData removeCertificateRequestData;
//                removeCertificateRequestData.certStr = root["data"]["certStr"].asString();
//                return BaseRequestMessage<RemoveCertificateRequestData>(code, removeCertificateRequestData);
//            case 3:
//                GetCertificateLifetimeRequestData getCertificateLifetimeRequestData;
//                getCertificateLifetimeRequestData.certStr = root["data"]["certStr"].asString();
//                return BaseRequestMessage<GetCertificateLifetimeRequestData>(code, getCertificateLifetimeRequestData);
//            case 4:
//                ExistsCertificateRequestData existsCertificateRequestData;
//                existsCertificateRequestData.certStr = root["data"]["certStr"].asString();
//                return BaseRequestMessage<ExistsCertificateRequestData>(code, existsCertificateRequestData);
//            default:
//                return BaseRequestMessage<std::string>(code, "");
//        }
//    }

protected:
    int code{0};
};

const int BaseRequestMessage::ADD_OR_UPDATE_CERTIFICATE = 1;
const int BaseRequestMessage::REMOVE_CERTIFICATE = 2;
const int BaseRequestMessage::GET_CERTIFICATE_LIFETIME = 3;
const int BaseRequestMessage::EXISTS_CERTIFICATE = 4;

#endif //MIRCERTIFICATIONMANAGER_BASEREQUESTMESSAGE_H
