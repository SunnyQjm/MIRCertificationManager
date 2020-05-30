//
// Created by mingj on 2020/5/30.
//

#include "utils/RedisUtil.h"
#include "utils/FileUtils.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    if (argc < 3) {
        cout << "Usage : ./installCert -f <filename> " << endl;
    }

    // 读取文件里的证书为字符串
    string certStr = FileUtils::readFile2String(argv[2]);
    cout << "Read cert: " << endl << certStr << endl;

    RedisUtil redisUtil;

}