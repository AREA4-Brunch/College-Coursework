#include "../h/MemoryAllocator.hpp"


// ==================================
// Constructor and Helpers:


kernel::MemoryAllocator::MemoryAllocator() {
    this->reset();
}

kernel::MemoryAllocator& kernel::MemoryAllocator::getInstance() {
    static kernel::MemoryAllocator instance;
    return instance;
}

void kernel::MemoryAllocator::reset() {
    // first address that can be returned to user,
    // also first address strictly greater than HEAP_START_ADDR and
    // divisible by MEM_BLOCK_SIZE
    const size_t heap_start = (size_t) HEAP_START_ADDR
                            - ((size_t) HEAP_START_ADDR % MEM_BLOCK_SIZE)
                            + MEM_BLOCK_SIZE;
    // end of heap (exclusive)
    const size_t heap_end = (size_t) HEAP_END_ADDR;

    if (heap_start > heap_end) {
        // was not provided enough space to align blocks
        this->head = nullptr;
        return;
    }

    // store head just before the second memory block
    this->head = (Header*) (heap_start - sizeof(Header::size));

    // set size to all blocks from head(except for size attr) to end of heap
    this->head->size = (heap_end - heap_start) / MEM_BLOCK_SIZE;
    this->head->next = nullptr;
}


// ==================================
// Public Methods:


void* kernel::MemoryAllocator::mallocBytes(size_t size) {
    return this->malloc(MemoryAllocator::bytesToBlocks(size));
}

void* kernel::MemoryAllocator::malloc(size_t size)
{
    if (!this->head) return nullptr;
    if (!size) return nullptr;

    // first fit:

    // try allocating from head:
    Header *cur = this->head;
    void *allocated_mem = this->allocateBlocks(cur, size);

    if (allocated_mem) {  // head had enough space
        this->head = cur;
        return allocated_mem;
    }

    // check the rest:
    for (Header *prev = this->head; cur; prev = cur, cur = cur->next) {
        if ((allocated_mem = this->allocateBlocks(cur, size))) {
            prev->next = cur;
            return allocated_mem;
        }
    }

    return nullptr;
}

int kernel::MemoryAllocator::free(void *data) {
    if (!data) return -1;

    Header *data_header = (Header *) ((size_t) data - sizeof(Header::size));

    // find the memory blocks right before and after the data
    Header *preceding = nullptr;
    Header *succeeding = this->head;
    while (succeeding && (size_t) succeeding < (size_t) data_header) {
        preceding = succeeding;
        succeeding = succeeding->next;
    }

    if (!preceding) {  // data header should be first
        this->head = data_header;
        if (!succeeding) return 1;  // list was empty, added new head

        if (this->areHeadersMergeable(data_header, succeeding)) {
            data_header->next = succeeding->next;
            data_header->size += succeeding->size + 1;

            return 2;  // merged old head with data_header as new head
        }

        // just link new head(data_header) and old head
        data_header->next = succeeding;

        return 3;  // prepended data_header
    }

    // data_header is not first:

    if (this->areHeadersMergeable(preceding, data_header)) {
        preceding->size += data_header->size + 1;

        if (succeeding && this->areHeadersMergeable(preceding, succeeding)) {
            preceding->size += succeeding->size + 1;
            preceding->next = succeeding->next;

            return 4;  // merged it with preceding and succeeding
        }

        return 5;  // merged it only with preceding
    }

    if (succeeding && this->areHeadersMergeable(data_header, succeeding)) {
        data_header->size += succeeding->size + 1;
        data_header->next = succeeding->next;
        preceding->next = data_header;

        return 6;  // merged it only with succeeding, succeeding != head
    }

    // data_header cannot be merged with anything
    // so just link it with preceding and succeeding
    preceding->next = data_header;
    data_header->next = succeeding;

    return 7;  // linked it somewhere after head
}


// ==================================
// Private Static Methods:


void* kernel::MemoryAllocator::allocateBlocks(Header* &header, const size_t requested_blocks) {
    if (!header) return nullptr;
    if (!requested_blocks) return nullptr;

    // convert bytes to blocks
    //size_t requested_blocks = (requested_bytes + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    // does not have enough space
    if (header->size < requested_blocks) return nullptr;

    // get beginning of heap memory user gets
    void *memory_start = (void*) ( (size_t) header + sizeof(Header::size) );

    // the amount of blocks is just right or has just 1 extra block
    // which cannot be available because header will require it
    if (header->size <= requested_blocks + 1) {
        header = header->next;  // update the ptr arg
        return memory_start;
    }

    // place header in the next block and update its data
    Header *leftover = (Header*) ( (size_t) header + MEM_BLOCK_SIZE * (requested_blocks + 1) );
    leftover->next = header->next;
    leftover->size = header->size - (requested_blocks + 1);
    header->size = requested_blocks;  // set size of allocated block
    header = leftover;  // update the ptr arg

    return memory_start;
}
