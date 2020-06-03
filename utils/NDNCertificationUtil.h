//
// Created by mingj on 2020/5/30.
//

#ifndef MIRCERTIFICATIONMANAGER_NDNCERTIFICATIONUTIL_H
#define MIRCERTIFICATIONMANAGER_NDNCERTIFICATIONUTIL_H

#include <ndn-cxx/encoding/buffer-stream.hpp>
#include <ndn-cxx/util/io.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>
#include <ndn-cxx/security/transform.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/data.hpp>
#include <string>
#include "RedisUtil.h"

/**
 * NDN证书相关的工具函数
 */
class NDNCertificationUtil {
public:
    /**
     * 验证一个包含证书的字符串是不是一个合法的NDN证书
     * @param certStr
     * @return  true    =>  certStr是一个合法的NDN证书字符串
     *          false   =>  certStr不是一个合法的NDN证书字符串
     */
    static bool isValidBlock(const std::string &certStr);

    /**
     * 连接到redis server
     * @param host  ip
     * @param port  端口
     * @return
     */
    bool connect(const std::string &host = "127.0.0.1", int port = 6379);

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
    int installCert(const std::string &certStr, long lifetime = -1, bool forceUpdate = false);

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
    int uninstallCert(const std::string &certStr);

    /**
     * 根据keyName查询redis，得到对应的证书字符串
     * @param keyName
     * @return
     */
    std::string getCertStrByKeyName(const std::string &keyName);

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
    long getCertLifetime(const std::string &certStr);

    /**
     * 判断某个证书是否存在
     * @param certStr
     * @return 如果存在则返回大于0的值，典型值为1
     */
    bool exists(const std::string &certStr);

    /**
     * 验证兴趣包签名是否有效
     * @param interest
     * @return
     */
    bool verifyInterest(const ndn::Interest &interest);

    /**
     * 验证数据包签名是否有效
     * @param data
     * @return
     */
    bool verifyData(const ndn::Data &data);

private:
    RedisUtil redisUtil;
};


#endif //MIRCERTIFICATIONMANAGER_NDNCERTIFICATIONUTIL_H
