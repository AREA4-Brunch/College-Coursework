#ifndef THREAD_SAFE_BUFFER_HPP
#define THREAD_SAFE_BUFFER_HPP


#include "../h/Semaphore.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/ControlFlow.hpp"
#include "../h/RISCV.hpp"
#include "../h/syscall_c.h"


namespace kernel {
namespace utils {


template<typename DTYPE>
class ThreadSafeBuffer
{

 public:


    // ==================================
    // Constructors:


    explicit ThreadSafeBuffer(const int capacity);

    /// Not thread safe.
    void* operator new(size_t size);


    // ==================================
    // Destructor:


    ~ThreadSafeBuffer();

    /// Not thread safe.
    void operator delete(void *data);


    // ==================================
    // Adding into the buffer:


    /**
     * Adds given data at the end of the buffer.
     * Is thread-safe.
     *
     * @param data
     * @return - 0 if everything was successful;
     *         - positive value if data was successfully stored,
     *           but semaphore signalling error occurred;
     *         - negative value if data was not stored;
     */
    int push(DTYPE &data);


    // ==================================
    // Removing from the buffer:


    /**
     * Removes and returns first element from the buffer.
     * Is thread-safe.
     *
     * @param err_code - stores into it:
     *                   - 0 if everything was successful;
     *                   - positive value if data was successfully retrieved,
     *                     but semaphore signalling error occurred;
     *                   - negative value if operation was unsuccessful;
     * @return First element from the buffer
     */
    DTYPE pop(int &err_code);


    // ==================================
    // Misc.


    /// Returns number of elements in the buffer.
    /// In case of error returns negative value.
    /// Is thread-safe.
    int size() const;

    /// Returns pointer to the first element in the buffer,
    /// in case buffer is not empty, returns nullptr otherwise.
    /// Is thread-safe, must be called outside interrupt
    /// routine, since it uses C API.
    DTYPE* front() const;

    /// Returns pointer to the last element in the buffer,
    /// in case buffer is not empty, returns nullptr otherwise.
    /// Is thread-safe, must be called outside interrupt
    /// routine, since it uses C API.
    DTYPE* back() const;


 private:


    DTYPE *buffer;
    const int capacity;
    int iter_start = 0;  // idx of first el in buffer
    int iter_end = 0;  // idx to place next available el in buffer

    mutable kernel::Semaphore space_available;
    mutable kernel::Semaphore elem_available;
    mutable kernel::Semaphore mutex_iter_start;
    mutable kernel::Semaphore mutex_iter_end;

};


}  // utils namespace
}  // kernel namespace



// ==================================
// ==================================
// ==================================
// Implementation:



// ==================================
// Constructors and Destructor:


template<typename DTYPE>
kernel::utils::ThreadSafeBuffer<DTYPE>::ThreadSafeBuffer(const int capacity)
    : buffer((DTYPE*) kernel::MemoryAllocator::getInstance().mallocBytes((capacity + 1) * sizeof(DTYPE))),
      capacity(capacity + 1), iter_start(0), iter_end(0),
      space_available(capacity), elem_available(0),
      mutex_iter_start(1), mutex_iter_end(1)
{

}

template<typename DTYPE>
void *kernel::utils::ThreadSafeBuffer<DTYPE>::operator new(size_t size) {
    auto &allocator = kernel::MemoryAllocator::getInstance();
    return allocator.mallocBytes(size);
}

template<typename DTYPE>
kernel::utils::ThreadSafeBuffer<DTYPE>::~ThreadSafeBuffer() {
    kernel::MemoryAllocator::getInstance().free(this->buffer);
}

template<typename DTYPE>
void kernel::utils::ThreadSafeBuffer<DTYPE>::operator delete(void *data) {
    auto &allocator = kernel::MemoryAllocator::getInstance();
    // destructor is called implicitly before operator delete
    //((ThreadSafeBuffer *) data)->~ThreadSafeBuffer();
    allocator.free(data);
}


// ==================================
// Adding into the buffer:


template<typename DTYPE>
int kernel::utils::ThreadSafeBuffer<DTYPE>::push(DTYPE &data) {
    auto &flow = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());

    int err_code = 0;

    // wait for end iter to become available
    // do not keep retrying to signal in case error occurred
    // not to stay too long in interrupt routine
    if (this->space_available.wait(flow) < 0) return err_code = -1;

    // wait for end iter to become available
    // do not keep retrying to signal in case error occurred
    // not to stay too long in interrupt routine
    if (this->mutex_iter_end.wait(flow) < 0) return err_code = -2;

    this->buffer[this->iter_end] = data;
    this->iter_end = (this->iter_end + 1) % this->capacity;

    // do not keep retrying to signal in case error occurred
    // not to stay too long in interrupt routine,
    // ignore the error because data was successfully stored
    if (this->mutex_iter_end.signal(flow) < 0) err_code = 1;

    // do not keep retrying to signal in case error occurred
    // not to stay too long in interrupt routine,
    // ignore the error because data was successfully stored
    if (this->elem_available.signal(flow) < 0) err_code = 2;

    return err_code;
}


// ==================================
// Removing from the buffer:


template<typename DTYPE>
DTYPE kernel::utils::ThreadSafeBuffer<DTYPE>::pop(int &err_code) {
    auto &flow = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());

    err_code = 0;  // success code

    // wait for item in the buffer to become available
    // do not keep retrying to signal in case error occurred
    // not to stay too long in interrupt routine
    if (this->elem_available.wait(flow) < 0) err_code = -1;

    // wait for start iter to become available
    // do not keep retrying to signal in case error occurred
    // not to stay too long in interrupt routine
    if (this->mutex_iter_start.wait(flow) < 0) err_code = -2;

    DTYPE data = this->buffer[this->iter_start];
    this->iter_start = (this->iter_start + 1) % this->capacity;

    // do not keep retrying to signal in case error occurred
    // not to stay too long in interrupt routine
    if (this->mutex_iter_start.signal(flow) < 0) err_code = 1;

    // do not keep retrying to signal in case error occurred
    // not to stay too long in interrupt routine
    if (this->space_available.signal(flow) < 0) err_code = 2;

    return data;
}


// ==================================
// Misc:


template<typename DTYPE>
int kernel::utils::ThreadSafeBuffer<DTYPE>::size() const {
    auto &flow = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());

    // in case of error return negative value
    if (this->mutex_iter_start.wait(flow) < 0) return -1;
    if (this->mutex_iter_end.wait(flow) < 0) return -2;

    const int size = this->iter_end >= this->iter_start ?
                     this->iter_end - this->iter_start
                   : this->capacity - this->iter_start + this->iter_end;

    while (this->mutex_iter_end.signal(flow) < 0);
    while (this->mutex_iter_start.signal(flow) < 0);

    return size;
}

template<typename DTYPE>
DTYPE* kernel::utils::ThreadSafeBuffer<DTYPE>::front() const {
    DTYPE *data = nullptr;

    // keep retrying to wait in case error occurred
    while (sem_wait((sem_t) &this->mutex_iter_start) < 0);
    while (sem_wait((sem_t) &this->mutex_iter_end) < 0);

    const int size = this->iter_end >= this->iter_start ?
                     this->iter_end - this->iter_start
                   : this->capacity - this->iter_start + this->iter_end;

    if (size > 0) data = &this->buffer[this->iter_start];
    // else data = nullptr

    // keep retrying to signal in case error occurred
    while (sem_wait((sem_t) &this->mutex_iter_end) < 0);
    while (sem_signal((sem_t) &this->mutex_iter_start) < 0);

    return data;
}

template<typename DTYPE>
DTYPE* kernel::utils::ThreadSafeBuffer<DTYPE>::back() const {
    DTYPE *data = nullptr;

    // keep retrying to wait in case error occurred
    while (sem_wait((sem_t) &this->mutex_iter_start) < 0);
    while (sem_wait((sem_t) &this->mutex_iter_end) < 0);

    const int size = this->iter_end >= this->iter_start ?
                     this->iter_end - this->iter_start
                    : this->capacity - this->iter_start + this->iter_end;

    if (size > 0) {
        if (this->iter_end == 0) data = &this->buffer[this->capacity - 1];
        else data = &this->buffer[this->iter_end - 1];
    }
    // else data = nullptr

    // keep retrying to signal in case error occurred
    while (sem_wait((sem_t) &this->mutex_iter_end) < 0);
    while (sem_signal((sem_t) &this->mutex_iter_start) < 0);

    return data;
}


#endif  // THREAD_SAFE_BUFFER_HPP
