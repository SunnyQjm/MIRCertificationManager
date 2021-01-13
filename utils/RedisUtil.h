//
// Created by mingj on 2020/5/30.
//

#ifndef MIRCERTIFICATIONMANAGER_REDISUTIL_H
#define MIRCERTIFICATIONMANAGER_REDISUTIL_H

#include <cstring>
#include <string>
#include <hiredis/hiredis.h>

class RedisUtil {
public:
    RedisUtil();

    virtual ~RedisUtil();

    /**
     * 连接到redis server
     * @param host  ip
     * @param port  端口
     * @return
     */
    bool connect(const std::string &host, int port, const std::string &passwd);

    /**
     * 从redis中根据指定的键取到指定的值
     * @param key
     * @return
     *      如果key对应的条目存在，则返回条目对应的值
     *      如果key对应的条目不存在，则会返回空字符串（所以如果要判定当前key对应的条目是否存在，则可以调用 RedisUtil::exists ）
     */
    std::string get(const std::string &key);

    /**
     * 设置或者更新键值对
     * @param key
     * @param value
     * @return
     */
    bool set(const std::string &key, const std::string &value);

    /**
     * 删除某个key对应的条目
     * @param key
     * @return 返回受影响的条目，比如：返回1表示成功删除一个key对应的条目
     */
    int del(const std::string &key);

    /**
     * 判断某个key对应的条目是否存在
     * @param key
     * @return 如果存在则返回大于0的值，典型值为1
     */
    int exist(const std::string &key);

    /**
     * 设置key对应条目的生存期，单位为毫秒；
     *
     * ps:  1. 如果一个key对应条目的生存期设置成功，则在经过指定的时间之后，该条目会过期，并且从redis中删除
     *      2. 默认情况下，没用调用本函数设置条目的生存期的话，条目是持久保存的，在被删除之前会一直存在于redis当中
     * @param key
     * @return 如果key对应的条目存在且设置生存期成功，则返回一个大于0的值；否则返回0
     */
    int setLifeTime(const std::string &key, long lifetime);

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
    long getRemainingTime(const std::string &key);


private:
    redisContext *_connect{};
    redisReply *_reply{};
};


#endif //MIRCERTIFICATIONMANAGER_REDISUTIL_H
