#ifndef MEMORY_ALLOCATOR_HPP
#define MEMORY_ALLOCATOR_HPP

#include "../lib/hw.h"


namespace kernel {


class MemoryAllocator final
{

 public:

    static MemoryAllocator& getInstance();

    // Forbid copies because of Singleton
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator= (const MemoryAllocator&) = delete;

    /**
     * Returns address of first block of space it allocates.
     *
     * @param size - number of blocks to allocate
     * @return address of first block it allocates.
     */
    void* malloc(size_t size);

    /**
     * Returns address of first block of space it allocates.
     *
     * @param size - number of bytes to allocate
     * @return address of first block it allocates.
     */
    void* mallocBytes(size_t size);

    /**
     * Labels given data as deallocated and returns positive
     * value in case of success, negative value otherwise.
     *
     * @param data - ptr returned by previously called malloc
     * @return - positive value in case of success,
     *           negative value otherwise.
     */
    int free(void *data);

    /// Returns smallest number of blocks needed to fit in
    /// given number of bytes.
    static size_t bytesToBlocks(const size_t bytes) {
        return (bytes + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    }


 private:


    struct Header {
        size_t size;  // number of available blocks
        Header *next;
    };


    // Attributes:

    // head of linked list of available blocks which is sorted
    // in ascending order by memory addresses
    Header *head = nullptr;

    // Constructor/Destructor and Singleton:

    MemoryAllocator();
    ~MemoryAllocator() = default;

    // Constructor helpers:

    /// Resets the instance.
    void reset();

    // Memory Management Helpers:

    /// Cuts off whole number of blocks from given header and
    /// returns its start as start of allocated memory. The rest
    /// accommodates to next available memory blocks if any left.
    /// Returns nullptr in case there are not enough continuous blocks available.
    static void* allocateBlocks(Header* &header, const size_t requested_blocks);

    inline static bool areHeadersMergeable(const Header *left, const Header *right);

};


}  // kernel namespace


// ==================================
// Inline Methods Implementation:


bool kernel::MemoryAllocator::areHeadersMergeable(const Header *left, const Header *right)
{
    return (size_t) left + (left->size + 1) * MEM_BLOCK_SIZE
        == (size_t) right;
}


#endif //MEMORY_ALLOCATOR_HPP
