#ifndef MEMMORY_POOL_H
#define MEMMORY_POOL_H

#include <cstdint>
#include <utility>
#include <new>

namespace mySource {
using PageInt = uint32_t;

constexpr PageInt INIT_SIZE = 4096;
constexpr PageInt GROW_SIZE = 4096;

/**
 * @brief Align to multiples of 8
 */
constexpr PageInt AlignTo(PageInt size, PageInt alignment = 8) {
    return (size + alignment - 1) & ~(alignment - 1);
}

struct alignas(8) BlockHeader {
    PageInt size;
    bool isFree;
    BlockHeader* next;
    BlockHeader(PageInt size, bool isFree, BlockHeader* next)
        : size(size), isFree(isFree), next(next) {}
    uint64_t GetDataAddress() const {
        return reinterpret_cast<uint64_t>(this + 1);
    }
};

class MemoryPage {
public:
    MemoryPage(PageInt size, PageInt growSize = GROW_SIZE)
        : _size(size), _growSize(growSize), _nextPage(nullptr) {
        _address = reinterpret_cast<uint64_t>(new uint8_t[size]);
        _blockList = AllocateBlockHeader(_address, size, true);
    }
    MemoryPage(const MemoryPage&) = delete;
    MemoryPage(MemoryPage&&) = delete;
    ~MemoryPage() {
        delete[] reinterpret_cast<uint8_t*>(_address);
    }

    uint64_t Allocate(PageInt size) {
        BlockHeader* cur = _blockList;
        BlockHeader* prev = nullptr;
        PageInt realSize = AlignTo(size + sizeof(BlockHeader));
        while(cur) {
            if (cur->isFree && cur->size >= realSize) {
                cur->isFree = false;
                // split the lock
                PageInt remainSize = cur->size - realSize;
                cur->size = realSize;
                if (remainSize > sizeof(BlockHeader)) {
                    cur->next = AllocateBlockHeader(cur->GetDataAddress() + size, remainSize, true);
                }
                return cur->GetDataAddress();
            }
            prev = cur;
            cur = cur->next;
        }
        if (_nextPage != nullptr) {
            _nextPage = new MemoryPage(_size, _growSize);
        }
        return _nextPage->Allocate(size);
    }
    bool Deallocate(uint64_t address) {
        if (address < _address || address >= _address + _size) return false;
        BlockHeader* cur = _blockList;
        while(cur) {
            if (cur->GetDataAddress() == address) {
                cur->isFree = true;
            }
        }
    }

protected:
    BlockHeader* AllocateBlockHeader(uint64_t startAddress, PageInt size, bool isFree, BlockHeader* next = nullptr) {
        return new (reinterpret_cast<BlockHeader*>(startAddress)) BlockHeader(size, isFree, next);
    }

private:
    uint64_t _address;
    PageInt _size;
    PageInt _growSize;
    MemoryPage* _nextPage{nullptr};
    BlockHeader* _blockList{nullptr};
};

class MemoryPool final {
public:
    ~MemoryPool() {
        delete _pageLinkList;
        _pageLinkList = nullptr;
    }
    static MemoryPool* GetInstance() {
        return &_instance;
    }
    template<typename T, typename... Args>
    T* MakePtr(Args&&... args) {
        return new (Allocate<T>()) T(std::forward<Args>(args)...);
    }
    template<typename T>
    T* Allocate() {
        return reinterpret_cast<T*>(_pageLinkList->Allocate(sizeof(T)));
    }
    template<typename T>
    bool Destroy(T* rawPtr) {
        if (rawPtr) rawPtr->~T();
        return _pageLinkList->Deallocate(reinterpret_cast<uint64_t>(rawPtr));
    }

private:
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;
    MemoryPool(PageInt initSize = INIT_SIZE, PageInt growSize = GROW_SIZE)
        : MemoryPool(initSize, growSize, nullptr) {}
    MemoryPool(PageInt initSize, PageInt growSize, MemoryPage* pageLinkList)
        : _initSize(AlignTo(initSize)), _growSize(AlignTo(growSize)), _pageLinkList(pageLinkList) {
        _pageLinkList = new MemoryPage(_initSize, _growSize);
    }

private:
    PageInt _initSize;
    PageInt _growSize;
    MemoryPage* _pageLinkList;
    static MemoryPool _instance;
};
MemoryPool MemoryPool::_instance;
}; // namespace mySource

#endif // !MEMMORY_POOL_H