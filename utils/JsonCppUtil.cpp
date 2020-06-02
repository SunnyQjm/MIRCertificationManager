//
// Created by mingj on 18-12-22.
//

#include "JsonCppUtil.h"

JsonCppUtil::JsonCppUtil(const string &jsonStr) {
    Json::Reader reader;

    //将根节点解析到root当中
    reader.parse(jsonStr, this->root);
}

string JsonCppUtil::getString(const string &key) {

    return root[key].asString();
}

int JsonCppUtil::getInt(const string &key) {
    return root[key].asInt();
}

unsigned long long JsonCppUtil::getull(const string &key) {
    return root[key].asUInt64();
}

bool JsonCppUtil::getBool(const string &key) {
    return root[key].asBool();
}

double JsonCppUtil::getDouble(const string &key) {
    return root[key].asDouble();
}

Json::Value JsonCppUtil::get(const string &key) {
    return root[key];
}
