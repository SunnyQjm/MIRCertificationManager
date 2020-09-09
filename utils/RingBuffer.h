//
// Created by sunny on 2020/6/10.
//

#ifndef MINSOCKETSTACK_CPP_RINGBUFFER_H
#define MINSOCKETSTACK_CPP_RINGBUFFER_H

#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>


#define CACHE_LINE_SIZE 64      // 缓存行大小

using namespace boost::interprocess;

class RingBuffer {

public:
    /**
     * RingBuffer 的元数据结构
     */
    struct RBMeta {
        char cachePad0[CACHE_LINE_SIZE]{};
        volatile long readIndex{0};
        volatile long writeIndex{0};
        char cachePad1[CACHE_LINE_SIZE - 2 * sizeof(long)]{};

        /////////////////////////// For Reader
        long localWriteIndex{0};
        long nextReadIndex{0};
        char cachePad2[CACHE_LINE_SIZE - 2 * sizeof(long)]{};
        /////////////////////////// End For Reader

        /////////////////////////// For Writer
        long localReadIndex{0};
        long nextWriteIndex{0};
        char cachePad3[CACHE_LINE_SIZE - 2 * sizeof(long)]{};
        /////////////////////////// End For Writer

        volatile bool isClose{false};  // 标记是否关闭，一个处于Close状态的共享内存对象读返回-1,写返回-2
    };

    explicit
    RingBuffer(uint64_t bufferSize = 4 * 1024 * 1024);

    /**
     * 打开一个共享内存
     * 1. 打开模式为create_only，如果对应名称的共享内存已经存在，则会返回false
     * @param shareMemoryName
     * @return
     */
    bool open(const std::string &shareMemoryName);

    /**
     * 连接一个共享内存
     * 1. 打开模式为open_only，如果对应名称的共享内存不存在，则会返回false
     * @param shareMemoryName
     * @return
     */
    bool connect(const std::string &shareMemoryName);

    /**
     * 移除一个当前创建的共享内存
     * @return
     */
    bool remove();

    /**
     * 返回当前RingBuffer内数据的大小
     * @return
     */
    uint64_t size();


    /**
     * 获取循环数组的下标
     * 1. 通过位运算计算，比模运算性能更佳；
     * 2. 要求bufferSize的大小位2^n
     *
     * @param from
     * @param size
     * @return
     */
    inline uint64_t nextVal(uint64_t from, uint64_t size) const {
        return (from + size) & (bufferSize - 1);
    }

    /**
     * 判断在一个长度为bufferSize的环形数组内，从下标位from位置向前行进length步到达to，会不会经过target(包括等于target)
     *

     第一种情形：
     -------------------------------------------------------------------------------------
         | |                | |             | |         | |                     | |
     -------------------------------------------------------------------------------------
          ^                  ^               ^           ^                       ^
          |                  |               |           |                       |
        target             from           target        to                      target

     第二种情形（绕过数组终点，回到起点继续走）：
     -------------------------------------------------------------------------------------
       | |         | |                | |                               | |         | |
     -------------------------------------------------------------------------------------
        ^           ^                  ^                                 ^           ^
        |           |                  |                                 |           |
      target       to               target                              from       target

     *
     * @param from
     * @param length
     * @param target
     * @return
     */
    inline bool isExceed(uint64_t from, uint64_t length, uint64_t to, uint64_t target) const {
        auto result = length >= bufferSize || length <= 0 ||
                      (
                              (to > from && target > from && target <= to)
                              ||
                              (to < from && (target > from || target <= to))
                      );
//        std::cout << "isExceed: [" << from << ", " << to << "], length: " << length << ", target: " << target << " => "
//                  << result << std::endl;
        return result;
    }

    inline bool
    isWriteExceed(uint64_t nextWriteIndex, uint64_t size, uint64_t afterNextWriteIndex, uint64_t nextReadIndex) const {
        return size >= bufferSize || size <= 0 ||
               (
                       (afterNextWriteIndex > nextWriteIndex && nextReadIndex > nextWriteIndex &&
                        nextReadIndex <= afterNextWriteIndex)
                       ||
                       (afterNextWriteIndex < nextWriteIndex &&
                        (nextReadIndex > nextWriteIndex || nextReadIndex <= afterNextWriteIndex))
               );
    }

    static inline bool
    isReadExceed(uint64_t nextReadIndex, uint64_t nextWriteIndex) {
        return nextReadIndex == nextWriteIndex;
    }

    /**
     * 往RingBuffer写入数据
     * @param dataBuffer    数据起始位置
     * @param size          数据的长度
     * @return  0 => 写入成功
     *         -1 => 写入失败（说明RingBuffer已满，请等待一段时间后再次调用）
     */
    int write(void *dataBuffer, uint64_t size);

    /**
     * 在RingBuffer中读取数据
     * @param dataBuffer    数据存入的起始位置
     * @param size          最长的长度
     * @return  -1 => 读取失败，发生错误
     *         >=0 => 本次读取到的size
     */
    int read(void *dataBuffer, uint64_t size);

    /**
     * 标记关闭该共享内存，不会实际执行销毁操作
     * 1. 只会在元数据中，把isClose标记为true；
     * 2. 然后其他人操作同一共享内存区域的时候，期望其读取元数据中isClose的状态；
     * 3. 如果isClose==true，期望其停止对共享内存的读写操作
     */
    void close();

    unsigned char *operator[](int i);

    ~RingBuffer();

private:
    std::string name;           // 共享内存的名字
    shared_memory_object sharedMemoryObject;
    mapped_region mmap;
    const uint64_t bufferSize{};             // 缓存的大小，只能是2^n => 这样可以使用位运算而不是模运算来计算索引

    ////////////////////下列数据存储在共享内存当中/////////////////////
    RBMeta *meta{};               // RingBuffer的元数据
    unsigned char *data{};        // RingBuffer的数据区
};


#endif //MINSOCKETSTACK_CPP_RINGBUFFER_H
