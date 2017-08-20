
#include <memory>
#include <cstring>
#include <atomic>
#include <array>
#include <cassert>
#include <forward_list>

class MemoryPool {

    static constexpr double MinBlockSize = 4096;
    static constexpr double RiseRatio = 1.5;
    static constexpr size_t MaxRiseLevel = 25;

    struct MemBlock {
        std::atomic<bool> idle;
        uint8_t p[];
    };

    using MemBlockList = std::forward_list<MemBlock*>;
    using MemBucketArr = std::array<std::shared_ptr<MemBlockList>, MaxRiseLevel>;

public:
    MemoryPool() {
        for(auto i = 0; i < MaxRiseLevel; ++i) {
            bucket[i] = std::make_shared<MemBlockList>();

            indexArr[i] = i == 0 ? 1 : indexArr[i - 1] * RiseRatio;
        }
    }
    ~MemoryPool() {}

    void * Allocate(size_t size) {
        assert(size > 0);

        size_t bucketIndex = sizeIndex(size);

        assert(bucketIndex < bucket.size());
        auto& blockList = bucket[bucketIndex];

        for(const auto & i : *blockList) {
            if(i->idle == true) {
//                std::cout << "reuse a block : " << MinBlockSize * indexArr[bucketIndex] << " at " << &i->p << std::endl;
                i->idle = false;
                return i->p;
            }
        }

        void *p = new char(MinBlockSize * indexArr[bucketIndex] + sizeof(std::atomic<bool>));
        auto block = static_cast<MemBlock*>(p);
        block->idle = false;
//        std::cout << "new a block : " << size << " "  <<  MinBlockSize * indexArr[bucketIndex] << " at " << &block->p << std::endl;
        blockList->push_front(block);
        return block->p;
    }

    void Deallocate(void * m) {
        auto block = reinterpret_cast<MemBlock*>((char*)m - sizeof(std::atomic<bool>));
        block->idle = true;
//        std::cout << "free a block at " << &block->p << std::endl;
    }

    void OutPutStatistics() {
        for(auto i = 0; i < bucket.size(); ++i) {
            std::cout << "block size : " << long(indexArr[i] * MinBlockSize) << " count : " << std::distance(bucket[i]->begin(), bucket[i]->end()) << std::endl;
        }
    }

private:
    size_t sizeIndex(size_t size) {
        auto i = std::upper_bound(indexArr.begin(), indexArr.end(), size / MinBlockSize);
        return i - indexArr.begin();
    }

private:
    MemBucketArr bucket;
    static std::array<double, MaxRiseLevel> indexArr;
};


std::array<double, MemoryPool::MaxRiseLevel> MemoryPool::indexArr;