#include "../h/syscall_c.h"
#include "../h/RISCV.hpp"
#include "../h/ControlFlow.hpp"
#include "../h/Console.hpp"


// ==============================
// Globals and macros:


sem_t SEM_USER_MAIN_DONE;


// ==============================
// Helpers:


void userMainWrapper(void*);


// ==============================
// Their Tests:


//#include "../test/userMain.cpp"
void userMain();


// ==============================
// RUNNER:



int main()
{
    // Block interruptions while preparing kernel
    auto &processor = kernel::RISCV::getInstance();
    //processor.setBlockInterruptions(true);

    auto &flow = kernel::ControlFlow::getInstance(processor);

    // create instance of console now before memory allocator can be
    // interrupted via another thread
    auto &console = kernel::Console::getInstance();

    // Create and set current flow as main/running thread:

//    thread_t main_thread = nullptr;
//    int err_code = thread_create(&main_thread, nullptr, nullptr);
//
//    if (err_code < 0) {  // failed to create console_consumer
//        return -1;
//    }

    //flow.setRunning((kernel::PCB *) main_thread);

    uint64 volatile SP;
    __asm__ volatile ("mv %[dest], sp" : [dest] "=r" (SP));

    kernel::PCB *main_thread = kernel::PCB::createPCB (
        nullptr, nullptr, reinterpret_cast<void*> (SP)
    );

    if (!main_thread) {  // failed to create main thread
        return -1;
    }

    // Switch to user mode via first ecall via thread_create
    // of console consumer or IDLE
    // and then set main as running and therefore allow change
    // of context on clock ticks and start user main afterwards
    int err_code = flow.initKernelThreadsAndUserMode(main_thread);
    if (err_code != 0) {  // failed to create kernel threads
        return err_code;
    }

    // Run user code and afterwards wait for it to be done
    // executing:
    sem_open(&SEM_USER_MAIN_DONE, 0);

    thread_t user_main_thread = nullptr;
    if (0 != thread_create(&user_main_thread, &userMainWrapper, nullptr)) {
        return -4;  // failed to create user main thread
    }

    // wait for the user main to end
    sem_wait(SEM_USER_MAIN_DONE);
    sem_close(SEM_USER_MAIN_DONE);

    // Kill any threads that user may have left:
    flow.terminateUserThreads();

    // wait for everything to be logged to the console:
    console.waitOutputStream();

    // set flag to exit loops in console_consumer and IDLE
    kernel::ControlFlow::terminateKernelThreads();


    return 0;
}



// ==============================
// Helpers:


void userMainWrapper(void *) {
    userMain();
    sem_signal(SEM_USER_MAIN_DONE);
}
