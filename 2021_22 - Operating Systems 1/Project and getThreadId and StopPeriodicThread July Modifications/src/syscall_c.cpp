#include "../h/syscall_c.h"
#include "../h/MemoryAllocator.hpp"  // bytesToBlocks func
#include "../h/RISCV.hpp"  // processor to call ABI on



void* mem_alloc(size_t size)
{
    auto &processor = kernel::RISCV::getInstance();

    // convert given number of bytes into number
    // of blocks on the heap to request
    const size_t size_in_blocks
            = kernel::MemoryAllocator::bytesToBlocks(size);

    // construct appropriate ABI function wrapper
    kernel::RISCV::ABI_Function malloc(
        kernel::RISCV::ABI_Function::ID::MALLOC
    );

    // store the arguments of the ABI call
    malloc.func_args[0] = size_in_blocks;

    // make the ABI call and fetch the return value
    void *ret_val = malloc.call(processor);
    return ret_val;
}


int mem_free(void* data)
{
    auto &processor = kernel::RISCV::getInstance();

    // construct appropriate ABI function wrapper
    kernel::RISCV::ABI_Function free(
        kernel::RISCV::ABI_Function::ID::FREE
    );

    // store the arguments of the ABI call
    free.func_args[0] = reinterpret_cast<uint64> (data);

    // make the ABI call and fetch the return value
    // which is void* (64-bit)
    uint64 raw_ret_val = reinterpret_cast<uint64>
            (free.call(processor));

    int err_code = static_cast<int> (raw_ret_val);
    return err_code;
}


int thread_create ( thread_t* handle,
                    void(*start_routine)(void*),
                    void* arg )
{
    auto &processor = kernel::RISCV::getInstance();

    // construct appropriate ABI function wrapper
    kernel::RISCV::ABI_Function thread_create(
        kernel::RISCV::ABI_Function::ID::THREAD_CREATE
    );

    // store the arguments of the ABI call
    thread_create.func_args[0] = reinterpret_cast<uint64> (handle);
    thread_create.func_args[1] = reinterpret_cast<uint64> (start_routine);
    thread_create.func_args[2] = reinterpret_cast<uint64> (arg);

    // find stack space somewhere on the heap
    void *stack_space_start = mem_alloc(kernel::PCB::DEFAULT_STACK_SIZE);
    if (!stack_space_start) return -50;  // malloc failed

    // inclusive end of stack
    void *stack_space_end = (void*) ((size_t) stack_space_start
                                    + kernel::PCB::DEFAULT_STACK_SIZE);
                                    //- 1UL);

    thread_create.func_args[3] = reinterpret_cast<uint64> (stack_space_end);

    // make the ABI call and fetch the return value
    // which is void* (64-bit)
    uint64 raw_ret_val = reinterpret_cast<uint64> (thread_create.call(processor));

    int err_code = static_cast<int> (raw_ret_val);
    return err_code;
}


int thread_exit() {
    auto &processor = kernel::RISCV::getInstance();

    // construct appropriate ABI function wrapper
    kernel::RISCV::ABI_Function thread_exit (
        kernel::RISCV::ABI_Function::ID::THREAD_EXIT
    );

    // store the arguments of the ABI call
    // nothing here

    // make the ABI call and fetch the return value
    // which is void* (64-bit)
    uint64 raw_ret_val = reinterpret_cast<uint64> (thread_exit.call(processor));

    int err_code = static_cast<int> (raw_ret_val);
    return err_code;
}


void thread_dispatch () {
    auto &processor = kernel::RISCV::getInstance();

    // construct appropriate ABI function wrapper
    kernel::RISCV::ABI_Function thread_dispatch (
        kernel::RISCV::ABI_Function::ID::THREAD_DISPATCH
    );

    // no args and no return value
    thread_dispatch.call(processor);
}


int sem_open(sem_t *handle, unsigned init) {
    auto &processor = kernel::RISCV::getInstance();

    kernel::RISCV::ABI_Function sem_open(
        kernel::RISCV::ABI_Function::ID::SEMAPHORE_OPEN
    );

    // store the arguments of the ABI call
    sem_open.func_args[0] = reinterpret_cast<uint64> (handle);
    sem_open.func_args[1] = static_cast<uint64> (init);

    // make the ABI call and fetch the return value
    // which is void* (64-bit)
    uint64 raw_ret_val = reinterpret_cast<uint64> (sem_open.call(processor));

    int err_code = static_cast<int> (raw_ret_val);
    return err_code;
}


int sem_close(sem_t handle) {
    auto &processor = kernel::RISCV::getInstance();

    kernel::RISCV::ABI_Function sem_close(
        kernel::RISCV::ABI_Function::ID::SEMAPHORE_CLOSE
    );

    // store the arguments of the ABI call
    sem_close.func_args[0] = reinterpret_cast<uint64> (handle);

    // make the ABI call and fetch the return value
    // which is void* (64-bit)
    uint64 raw_ret_val = reinterpret_cast<uint64> (sem_close.call(processor));

    int err_code = static_cast<int> (raw_ret_val);
    return err_code;
}


int sem_wait(sem_t id) {
    auto &processor = kernel::RISCV::getInstance();

    kernel::RISCV::ABI_Function sem_wait(
        kernel::RISCV::ABI_Function::ID::SEMAPHORE_WAIT
    );

    // store the arguments of the ABI call
    sem_wait.func_args[0] = reinterpret_cast<uint64> (id);

    // make the ABI call and fetch the return value
    // which is void* (64-bit)
    uint64 raw_ret_val = reinterpret_cast<uint64> (sem_wait.call(processor));

    int err_code = static_cast<int> (raw_ret_val);
    return err_code;
}


int sem_signal(sem_t id) {
    auto &processor = kernel::RISCV::getInstance();

    kernel::RISCV::ABI_Function sem_signal(
        kernel::RISCV::ABI_Function::ID::SEMAPHORE_SIGNAL
    );

    // store the arguments of the ABI call
    sem_signal.func_args[0] = reinterpret_cast<uint64> (id);

    // make the ABI call and fetch the return value
    // which is void* (64-bit)
    uint64 raw_ret_val = reinterpret_cast<uint64> (sem_signal.call(processor));

    int err_code = static_cast<int> (raw_ret_val);
    return err_code;
}


int time_sleep(time_t sleep_duration) {  // duration in clock ticks
    auto &processor = kernel::RISCV::getInstance();

    kernel::RISCV::ABI_Function time_sleep(
        kernel::RISCV::ABI_Function::ID::TIME_SLEEP
    );

    // store the arguments of the ABI call
    time_sleep.func_args[0] = reinterpret_cast<uint64> (sleep_duration);

    // make the ABI call and fetch the return value
    // which is void* (64-bit)
    uint64 raw_ret_val = reinterpret_cast<uint64> (time_sleep.call(processor));

    int err_code = static_cast<int> (raw_ret_val);
    return err_code;
}

char getc() {
    auto &processor = kernel::RISCV::getInstance();

    kernel::RISCV::ABI_Function getc(
        kernel::RISCV::ABI_Function::ID::GETC
    );

    // store the arguments of the ABI call
    // nothing here

    // make the ABI call and fetch the return value
    // which is void* (64-bit)
    uint64 raw_ret_val = reinterpret_cast<uint64> (getc.call(processor));

    char read_char = static_cast<char> (raw_ret_val);
    return read_char;
}

void putc(char chr) {
    auto &processor = kernel::RISCV::getInstance();

    kernel::RISCV::ABI_Function putc(
        kernel::RISCV::ABI_Function::ID::PUTC
    );

    // store the arguments of the ABI call
    putc.func_args[0] = static_cast<uint64> (chr);

    // make the ABI call and fetch the return value
    // which is void* (64-bit)

    // no return value for this function
    putc.call(processor);
}


int getThreadId() {
    auto &processor = kernel::RISCV::getInstance();

    kernel::RISCV::ABI_Function get_thread_id (
        kernel::RISCV::ABI_Function::ID::GET_THREAD_ID
    );

    uint64 raw_ret_val = reinterpret_cast<uint64> (get_thread_id.call(processor));
    int running_id = static_cast<int> (raw_ret_val);
    return running_id;
}


void thread_kill(thread_t handle) {
    auto &processor = kernel::RISCV::getInstance();

    kernel::RISCV::ABI_Function thread_kill(
        kernel::RISCV::ABI_Function::ID::THREAD_KILL
    );

    // store the arguments of the ABI call
    thread_kill.func_args[0] = reinterpret_cast<uint64> (handle);

    thread_kill.call(processor);
}
