#include <iostream>
#include "utils/RedisUtil.h"
#include "Server.h"

/**
 *
1. 接收区块链下发的登录信息，加载相应用户的证书到redis数据库中。下发登录信息应包含登录超时时间。

2. MIR收到登录请求后，使用兴趣包加载证书，兴趣包可以从缓存中得到证书，不然兴趣包会被发到区块链。

3. 修改NFD中加载证书验证网络分组的过程，改为从redis数据库中加载证书。

4. 把退出登录的功能也做一做。即从redis中删除证书。加载证书和删除证书的代码不写到NFD中，应单独开发。
 * @return
 */

using namespace std;

int main() {

    Server server;
    server.run();

//    AddOrUpdateCertificateRequestMessage addOrUpdateCertificateRequestMessage;
//    addOrUpdateCertificateRequestMessage.parse("{\n"
//                                               "\"code\": 1,\n"
//                                               "\"data\": {\n"
//                                               "\"certStr\": \"证书的base64编码字符串\",\n"
//                                               "\"lifetime\": 1000,\n"
//                                               "\"forceUpdate\": false\n"
//                                               "}\n"
//                                               "}");
//    cout << addOrUpdateCertificateRequestMessage.getCertStr() << endl;
//    RedisUtil redisUtil;
//    redisUtil.connect("127.0.0.1", 6379);
//    std::cout << redisUtil.set("test", "1234") << std::endl;
//    auto result = redisUtil.get("test1");
//    std::cout << result << std::endl;
//    std::cout << "exists: " << redisUtil.exist("test") << std::endl;
//    redisUtil.del("test");
//    std::cout << "exists: " << redisUtil.exist("test") << std::endl;
//    return 0;
}
