#include "../h/Semaphore.hpp"
#include "../h/PCB.hpp"
#include "../h/syscall_c.h"  // for calls outside interrupt routine


// ==================================
// Constructors:


void *kernel::Semaphore::operator new(size_t size) {
    auto &allocator = kernel::MemoryAllocator::getInstance();
    return allocator.mallocBytes(size);
}

void* kernel::Semaphore::operator new(size_t size, void *memory) {
    return (kernel::Semaphore *) memory;
}


// ==================================
// Getters:


int kernel::Semaphore::getValue() const {
    return this->counter;
}


// ==================================
// Destructor:


kernel::Semaphore::~Semaphore() {
    auto &scheduler = kernel::Scheduler::getInstance();

    while (!this->queue_blocked.empty()) {
        auto cur_pcb = this->queue_blocked[0];
        this->queue_blocked.pop_front();

        cur_pcb->setWasSemaphoreClosedWhileWaiting(true);
        scheduler.pushReady(cur_pcb);
    }
}

void kernel::Semaphore::operator delete(void *data) {
    auto &allocator = kernel::MemoryAllocator::getInstance();
    // destructor is called implicitly before operator delete
    //((Semaphore *) data)->~Semaphore();
    allocator.free(data);
}


// ==================================
// Public Methods:


int kernel::Semaphore::wait(kernel::ControlFlow &flow) {
    if (!flow.isInInterruptRoutine()) return sem_wait((sem_t) this);

    if (--this->counter < 0) return this->block(flow);
    return 0;
}

int kernel::Semaphore::signal(kernel::ControlFlow &flow) {
    if (!flow.isInInterruptRoutine()) return sem_signal((sem_t) this);

    if (++this->counter <= 0) return this->unblock(flow);
    return 0;
}


// ==================================
// Protected Methods:


int kernel::Semaphore::block(kernel::ControlFlow &flow) {
    // get current running PCB and add to blocked queue
    kernel::PCB* const running = flow.getRunning();

    this->queue_blocked.push_back(running);

    // change current running to next ready in scheduler
    flow.setNextRunning();

    // reset the elapsed time of the running PCB
    // flow.elapsed_time = 0UL;  // done in yield

    // yield:
    flow.yield(running, flow.getRunning());
    flow.setIsInInterruptRoutine(true);

    int err_code = running->getWasSemaphoreClosedWhileWaiting() ?
                   -1  // failure because semaphore was destroyed
                 : 0;  // success

    running->setWasSemaphoreClosedWhileWaiting(false);

    return err_code;
}

int kernel::Semaphore::unblock(kernel::ControlFlow &flow) {
    if (this->queue_blocked.empty()) {  // there is nothing to unblock
//        ::utils::logging::printlnString("ERROR: Blocked queue is empty.");
        return -1;
    }

    auto pcb_to_unblock = this->queue_blocked[0];
    this->queue_blocked.pop_front();
    kernel::Scheduler::getInstance().pushReady(pcb_to_unblock);

    return 0;
}
