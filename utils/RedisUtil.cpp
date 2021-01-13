//
// Created by mingj on 2020/5/30.
//

#include "RedisUtil.h"
#include <iostream>

RedisUtil::RedisUtil() = default;

RedisUtil::~RedisUtil() {
    redisFree(this->_connect);
    this->_connect = nullptr;
    this->_connect = nullptr;
}

/**
 * 连接到redis server
 * @param host  ip
 * @param port  端口
 * @return
 */
bool RedisUtil::connect(const std::string &host, int port, const std::string &passwd) {
    // 连接到指定的redis服务器
    this->_connect = redisConnect(host.c_str(), port);
    if (this->_connect == nullptr || this->_connect->err) {
        std::cerr << "connect error: " << this->_connect->errstr << std::endl;
        return false;
    }
    auto reply = (redisReply *)redisCommand(this->_connect, "AUTH %s", passwd.c_str());
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cerr << "Redis认证失败" << std::endl;
        return false;
    }
    return true;
}

/**
 * 从redis中根据指定的键取到指定的值
 * @param key
 * @return
 *      如果key对应的条目存在，则返回条目对应的值
 *      如果key对应的条目不存在，则会返回空字符串（所以如果要判定当前key对应的条目是否存在，则可以调用 RedisUtil::exists ）
 */
std::string RedisUtil::get(const std::string &key) {
    this->_reply = static_cast<redisReply *>(redisCommand(this->_connect, "GET %s", key.c_str()));
    std::string str = this->_reply->str;
    if (this->_reply->str == nullptr) {
        return std::string();
    }
    freeReplyObject(this->_reply);
    return str;
}

/**
 * 设置或者更新键值对
 * @param key
 * @param value
 * @return
 */
bool RedisUtil::set(const std::string &key, const std::string &value) {
    this->_reply = static_cast<redisReply *>(redisCommand(this->_connect, "SET %s %s", key.c_str(), value.c_str()));
    return std::strcmp(this->_reply->str, "OK") == 0;
}

/**
 * 删除某个key对应的条目
 * @param key
 * @return 返回受影响的条目，比如：返回1表示成功删除一个key对应的条目
 */
int RedisUtil::del(const std::string &key) {
    this->_reply = static_cast<redisReply *>(redisCommand(this->_connect, "DEL %s", key.c_str()));
    return static_cast<int>(this->_reply->integer);
}


/**
 * 判断某个key对应的条目是否存在
 * @param key
 * @return 如果存在则返回大于0的值，典型值为1
 */
int RedisUtil::exist(const std::string &key) {
    this->_reply = static_cast<redisReply *>(redisCommand(this->_connect, "EXISTS %s", key.c_str()));
    return static_cast<int>(this->_reply->integer);
}

/**
 * 设置key对应条目的生存期，单位为毫秒；
 *
 * ps:  1. 如果一个key对应条目的生存期设置成功，则在经过指定的时间之后，该条目会过期，并且从redis中删除
 *      2. 默认情况下，没用调用本函数设置条目的生存期的话，条目是持久保存的，在被删除之前会一直存在于redis当中
 * @param key
 * @return 如果key对应的条目存在且设置生存期成功，则返回一个大于0的值；否则返回0
 */
int RedisUtil::setLifeTime(const std::string &key, long lifetime) {
    std::cout << "setLifeTime: <" << key << ", " << lifetime << ">" << std::endl;
    this->_reply = static_cast<redisReply *>(redisCommand(this->_connect, "PEXPIRE %s %ld", key.c_str(), lifetime));
    return static_cast<int>(this->_reply->integer);
}

/**
 * 获取key对应条目的剩余存活时间，单位为毫秒；
 *
 * ps: 详情参考redis-cli的PTTL命令 => https://www.runoob.com/redis/keys-pttl.html
 *
 * @param key
 * @return
 *      -2 => 返回-2表示key对应的条目不存在（可能是本来就没有这个条目，或者本来有一个条目，但是因为设置了存活期，其存活期已到，被移除了）
 *      -1 => 返回-1表示key对应的条目存在，但是没有设置过存活期，是持久保存的
 *    >= 0 => 返回大于等于0的值表示key对应的条目存在，且其剩余的存活时间为返回的值，单位为毫秒
 */
long RedisUtil::getRemainingTime(const std::string &key) {
    this->_reply = static_cast<redisReply *>(redisCommand(this->_connect, "PTTL %s", key.c_str()));
    return static_cast<int>(this->_reply->integer);
}