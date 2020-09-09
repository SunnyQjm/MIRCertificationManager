//
// Created by sunny on 2020/6/10.
//

#include "RingBuffer.h"
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <semaphore.h>
#include <cstring>

RingBuffer::RingBuffer(uint64_t bufferSize) :
        bufferSize(bufferSize) {
}

RingBuffer::~RingBuffer() {
    // 移除共享内存
    sharedMemoryObject.remove(name.c_str());
}

bool RingBuffer::open(const std::string &shareMemoryName) {
    try {
        this->name = shareMemoryName;

        // 创建共享内存
        sharedMemoryObject = shared_memory_object(create_only, this->name.c_str(), read_write);

        uint64_t shareSize = sizeof(sem_t) + sizeof(RBMeta) + bufferSize;

        // 设置共享内存的大小
        sharedMemoryObject.truncate(shareSize);

        // 将共享内存映射到当前进程
        mmap = mapped_region(sharedMemoryObject, read_write);

        this->meta = reinterpret_cast<RBMeta *>((unsigned char *) mmap.get_address() + sizeof(sem_t));
        this->data = (unsigned char *) mmap.get_address() + sizeof(sem_t) + sizeof(RBMeta);

        // 使用信号量进行同步，保证本次创建所有初始化完成之前，其它想要连接到这个共享内存的进程持续等待，直到初始化完成，发出信号量
        auto *sem = (sem_t *) mmap.get_address();
        if (sem_init(sem, 1, 0) == -1) {
            std::cerr << "初始化信号量失败 => sem_init()" << std::endl;
            sharedMemoryObject.remove(this->name.c_str());
            return false;
        }
        meta->readIndex = 0;
        meta->writeIndex = 0;
        meta->localReadIndex = 0;
        meta->nextWriteIndex = 0;
        meta->localWriteIndex = 0;
        meta->nextReadIndex = 0;
        memset(this->data, 0, bufferSize);
        sem_post(sem);

    } catch (...) {
        std::cerr << "创建共享内存失败" << std::endl;
        sharedMemoryObject.remove(this->name.c_str());
        return false;
    }
    return true;
}

bool RingBuffer::connect(const std::string &shareMemoryName) {
    try {
        this->name = shareMemoryName;

        // 创建共享内存
        sharedMemoryObject = shared_memory_object(open_only, this->name.c_str(), read_write);

        uint64_t shareSize = sizeof(sem_t) + sizeof(RBMeta) + bufferSize;

        // 设置共享内存的大小
        sharedMemoryObject.truncate(shareSize);

        // 将共享内存映射到当前进程
        mmap = mapped_region(sharedMemoryObject, read_write);

        this->meta = reinterpret_cast<RBMeta *>((unsigned char *) mmap.get_address() + sizeof(sem_t));
        this->data = (unsigned char *) mmap.get_address() + sizeof(sem_t) + sizeof(RBMeta);

        auto *sem = (sem_t *) mmap.get_address();
        if (sem_wait(sem) == -1) {
            std::cerr << "等待信号量失败 => sem_wait()" << std::endl;
            sharedMemoryObject.remove(this->name.c_str());
            return false;
        }

        sem_post(sem);
    } catch (const std::exception& e) {
        std::cerr << "连接共享内存失败: " << e.what() << std::endl;
        sharedMemoryObject.remove(this->name.c_str());
        return false;
    }
    return true;
}

unsigned char *RingBuffer::operator[](int i) {
    return this->data + i;
}

uint64_t RingBuffer::size() {
//    std::cout << "nextWriteIndex: " << this->meta->nextWriteIndex << ", nextReadIndex：" << this->meta->nextReadIndex
//              << std::endl;
    if (this->meta->nextWriteIndex < this->meta->nextReadIndex) {
        return this->meta->nextWriteIndex - this->meta->nextReadIndex + bufferSize;
    } else {
        return this->meta->nextWriteIndex - this->meta->nextReadIndex;
    }
}

int RingBuffer::write(void *dataBuffer, uint64_t size) {
    if (this->meta->isClose) {
        return -2;
    }
    try {
        auto afterNextWriterIndex = nextVal(this->meta->nextWriteIndex, size);
        if (this->isWriteExceed(this->meta->nextWriteIndex, size, afterNextWriterIndex, this->meta->localReadIndex)) {
            // RingBuffer已满，无法再继续写入
            if (this->isWriteExceed(this->meta->nextWriteIndex, size, afterNextWriterIndex, this->meta->readIndex)) {
                return -1;
            }

            // 同步写者记录的localReadIndex和实际的readIndex
            this->meta->localReadIndex = this->meta->readIndex;
        }

        if (afterNextWriterIndex < this->meta->nextWriteIndex) {        // 处理回环绕回情况下的写入
            std::cout << "write 回环" << std::endl;
            auto firstSize = bufferSize - this->meta->nextWriteIndex;
            auto remainSize = size - firstSize;
            memcpy(this->data + this->meta->nextWriteIndex, dataBuffer, firstSize);
            memcpy(this->data, (void *) (((char *) dataBuffer) + firstSize), remainSize);
        } else {
            memcpy(this->data + this->meta->nextWriteIndex, dataBuffer, size);
        }
//        std::cout << "write => [" << this->meta->nextWriteIndex << " -> " << size << "]" << std::endl;

        this->meta->nextWriteIndex = afterNextWriterIndex;
        this->meta->writeIndex = this->meta->nextWriteIndex;
    } catch (...) {
        std::cerr << "fuck write error" << std::endl;
        return -1;
    }
    return 0;
}

int RingBuffer::read(void *dataBuffer, uint64_t size) {
    if (this->meta->isClose) {
        return -1;
    }
    try {
//        std::cout << "nextReadIndex: " << this->meta->nextReadIndex << ", localWriteIndex: "
//                  << this->meta->localWriteIndex << ", writeIndex: " << this->meta->writeIndex << std::endl;
        if (this->isReadExceed(this->meta->nextReadIndex, this->meta->localWriteIndex)) {
            // RingBuffer已空，无法再继续写入
            if (this->isReadExceed(this->meta->nextReadIndex, this->meta->writeIndex)) {
                return 0;
            }

            // 同步读者记录的localWriteIndex和实际的writeIndex
            this->meta->localWriteIndex = this->meta->writeIndex;
        }

        // 获取当前可读的size
        auto currentSize = this->size();
        auto canReadSize = currentSize > size ? size : currentSize;
        auto afterNextReadIndex = nextVal(this->meta->nextReadIndex, canReadSize);

        if (afterNextReadIndex < this->meta->nextReadIndex) {      // 处理回环情况
            std::cout << "read 回环" << std::endl;
            auto firstSize = bufferSize - this->meta->nextReadIndex;
            auto remainSize = canReadSize - firstSize;
            memcpy(dataBuffer, this->data + this->meta->nextReadIndex, firstSize);
            memcpy((void *) (((char *) dataBuffer) + firstSize), this->data, remainSize);
        } else {
            memcpy(dataBuffer, this->data + this->meta->nextReadIndex, canReadSize);
        }

//        std::cout << "read => [" << this->meta->nextReadIndex << "(" << this->meta->writeIndex << ")" << " -> "
//                  << canReadSize << "]" << std::endl;
        this->meta->nextReadIndex = this->nextVal(this->meta->nextReadIndex, canReadSize);
        this->meta->readIndex = this->meta->nextReadIndex;
        return canReadSize;
    } catch (...) {
        std::cerr << "fuck read error" << std::endl;
    }
    return -1;
}

bool RingBuffer::remove() {
    return sharedMemoryObject.remove(this->name.c_str());
}

void RingBuffer::close() {
    this->meta->isClose = true;
}
