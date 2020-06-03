//
// Created by mingj on 2020/5/30.
//

#include "NDNCertificationUtil.h"
#include <iostream>

/**
 * 验证一个包含证书的字符串是不是一个合法的NDN证书
 * @param certStr
 * @return  true    =>  certStr是一个合法的NDN证书字符串
 *          false   =>  certStr不是一个合法的NDN证书字符串
 */
bool NDNCertificationUtil::isValidBlock(const std::string &certStr) {
    namespace t = ndn::security::transform;
    try {
        ndn::OBufferStream os;
        std::istringstream is(certStr);
        t::streamSource(is) >> t::stripSpace("\n") >> t::base64Decode(false)
                            >> t::streamSink(os);
        ndn::Block b(os.buf());
    } catch (...) {
        return false;
    }
    return true;
}

/**
 * 连接到redis server
 * @param host  ip
 * @param port  端口
 * @return
 */
bool NDNCertificationUtil::connect(const std::string &host, int port) {
    return redisUtil.connect(host, port);
}

/**
 * 安装证书操作
 * 1. 传入一个NDN生成的证书base64编码后的字符串；
 * 2. 首先会验证是否是一个格式正确的证书；
 * 3. 尝试将证书的信息存到redis中
 * @param certStr       NDN生成的证书base64编码后的字符串
 * @param forceUpdate   强制更新（如果指定强制更新，则证书已经存在的情况下会覆盖旧的记录）
 * @return 0  -> 成功
 *         -1 -> 证书无效
 *         -2 -> 已经存在
 */
int NDNCertificationUtil::installCert(const std::string &certStr, long lifetime, bool forceUpdate) {
    // 首先验证证书是否有效
    if (!isValidBlock(certStr))
        return -1;
    // 接着构造证书
    std::stringstream ss(certStr);
    ndn::security::v2::Certificate cert =
            *(ndn::io::load<ndn::security::v2::Certificate>(ss));
    if (!forceUpdate && redisUtil.exist(cert.getKeyName().toUri()) > 0) {
        return -2;
    }
    std::cout << cert.getKeyName() << " => " << certStr << std::endl;
    int result = redisUtil.set(cert.getKeyName().toUri(), certStr) ? 0 : -3;
    if (lifetime > 0) {
        std::cout << "setLifetime: " << lifetime << std::endl << "result: "
                  << redisUtil.setLifeTime(cert.getKeyName().toUri(), lifetime) << std::endl;
    }
    return result;
}

/**
 * 卸载证书操作
 * 1. 传入一个NDN生成的证书base64编码后的字符串；
 * 2. 首先会验证是否是一个格式正确的证书；
 * 3. 尝试根据证书中的Name字段查询redis，并移除
 * @param certStr NDN生成的证书base64编码后的字符串
 * @return  >=0  -> 成功，其值表示redis中受影响的条目
 *          -1 -> 证书无效
 *          -2 -> 没有对应的条目
 */
int NDNCertificationUtil::uninstallCert(const std::string &certStr) {
    // 首先验证证书是否有效
    if (!isValidBlock(certStr))
        return -1;
    // 接着构造证书
    std::stringstream ss(certStr);
    ndn::security::v2::Certificate cert =
            *(ndn::io::load<ndn::security::v2::Certificate>(ss));
    return redisUtil.del(cert.getKeyName().toUri());
}

/**
 * 根据keyName查询redis，得到对应的证书字符串
 * @param keyName
 * @return
 *      如果key对应的条目存在，则返回条目对应的值
 *      如果key对应的条目不存在，则会返回空字符串（所以如果要判定当前key对应的条目是否存在，则可以调用 RedisUtil::exists ）
 */
std::string NDNCertificationUtil::getCertStrByKeyName(const std::string &keyName) {
    return redisUtil.get(keyName);
}

/**
 * 验证兴趣包签名是否有效
 * @param interest
 * @return
 */
bool NDNCertificationUtil::verifyInterest(const ndn::Interest &interest) {
    // 首先提取兴趣包的签名信息
    std::string keyLocatorStr;
    try {
        ndn::SignatureInfo signInfo(interest.getName()[ndn::signed_interest::POS_SIG_INFO].blockFromValue());
        //SignatureInfo signInfo(interest.getName[-2]
        keyLocatorStr = signInfo.getKeyLocator().getName().toUri();
    } catch (...) {
        return false;
    }

    //  接着从redis中取出对应的证书，如果有的话
    std::string certStr = this->getCertStrByKeyName(keyLocatorStr);
    if (certStr.size() <= 10) return false;

    // 接着用证书验证兴趣包的签名
    std::stringstream ss(certStr);
    ndn::security::v2::Certificate cert =
            *(ndn::io::load<ndn::security::v2::Certificate>(ss));
    auto result = ndn::security::verifySignature(interest, cert);
    return result;
}

/**
 * 验证数据包签名是否有效
 * @param data
 * @return
 */
bool NDNCertificationUtil::verifyData(const ndn::Data &data) {
    // 首先提取数据包的签名信息
    std::string keyLocatorStr;
    try {
        keyLocatorStr = data.getSignature().getKeyLocator().getName().toUri();
    } catch (...) {
        return false;
    }

    //  接着从redis中取出对应的证书，如果有的话
    std::string certStr = this->getCertStrByKeyName(keyLocatorStr);
    if (certStr.size() <= 10) return false;

    // 接着用证书验证兴趣包的签名
    std::stringstream ss(certStr);
    ndn::security::v2::Certificate cert =
            *(ndn::io::load<ndn::security::v2::Certificate>(ss));

    auto result = ndn::security::verifySignature(data, cert);
    return result;
}

/**
 * 获取证书剩余存活时间，单位为毫秒；
 *
 *
 * @param certStr
 * @return
 *      -2 => 返回-2表示key对应的条目不存在（可能是本来就没有这个条目，或者本来有一个条目，但是因为设置了存活期，其存活期已到，被移除了）
 *      -1 => 返回-1表示key对应的条目存在，但是没有设置过存活期，是持久保存的
 *    >= 0 => 返回大于等于0的值表示key对应的条目存在，且其剩余的存活时间为返回的值，单位为毫秒
 */
long NDNCertificationUtil::getCertLifetime(const std::string &certStr) {
    // 首先验证证书是否有效
    if (!isValidBlock(certStr))
        return -1;
    // 接着构造证书
    std::stringstream ss(certStr);
    ndn::security::v2::Certificate cert =
            *(ndn::io::load<ndn::security::v2::Certificate>(ss));
    return redisUtil.getRemainingTime(cert.getName().toUri());
}

bool NDNCertificationUtil::exists(const std::string &certStr) {
    // 首先验证证书是否有效
    if (!isValidBlock(certStr))
        return -1;
    // 接着构造证书
    std::stringstream ss(certStr);
    ndn::security::v2::Certificate cert =
            *(ndn::io::load<ndn::security::v2::Certificate>(ss));
    return redisUtil.exist(cert.getName().toUri()) > 0;
}
