//
// Created by mingj on 18-12-22.
//
#ifndef IP_NDN_STACK_CPP_JSONCPPHELPER_H
#define IP_NDN_STACK_CPP_JSONCPPHELPER_H

#include <jsoncpp/json/json.h>
#include <iostream>
#include <fstream>

using namespace std;

class JsonCppUtil {
public:
    explicit JsonCppUtil(const string &jsonStr);

    string getString(const string &key);

    int getInt(const string &key);

    unsigned long long getull(const string &key);

    bool getBool(const string &key);

    double getDouble(const string &key);

    Json::Value get(const string &key);

private:
    Json::Value root;
};


#endif //IP_NDN_STACK_CPP_JSONCPPHELPER_H
