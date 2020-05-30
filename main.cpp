#include <iostream>
#include "utils/RedisUtil.h"

int main() {
    RedisUtil redisUtil;
    redisUtil.connect("127.0.0.1", 6379);
    std::cout << redisUtil.set("test", "1234") << std::endl;
    auto result = redisUtil.get("test1");
    std::cout << result << std::endl;
    std::cout << "exists: " << redisUtil.exist("test") << std::endl;
    redisUtil.del("test");
    std::cout << "exists: " << redisUtil.exist("test") << std::endl;
    return 0;
}
