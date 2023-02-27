#include "../h/ControlFlow.hpp"
#include "../h/RISCV.hpp"
#include "../h/PCB.hpp"
#include "../h/syscall_c.h"  // thread_create
#include "../h/Console.hpp"  // for console consumer body
#include "../h/Semaphore.hpp"  // for periodic threads
#include "../h/syscall_cpp.hpp"  // for periodic threads


// ==================================
// Static Variables Initialization:


bool kernel::ControlFlow::is_kernel_terminate_pending = false;
kernel::PCB* kernel::ControlFlow::console_consumer = nullptr;


// ==================================
// Constructor and Helpers:


kernel::ControlFlow::ControlFlow(kernel::RISCV &processor,
                                 kernel::Scheduler &scheduler)
    : processor(processor), scheduler(&scheduler),
      periodic_thread_counter_mutex(new kernel::Semaphore(1)),
      periodic_thread_killer_sem(new kernel::Semaphore(0)),
      periodic_threads(new kernel::utils::LinkedList<::PeriodicThread*>())
{

}

kernel::ControlFlow &kernel::ControlFlow::getInstance(kernel::RISCV &processor) {
    static kernel::ControlFlow instance(processor,
                                        kernel::Scheduler::getInstance());
    return instance;
}


// ==================================
// Destructor:


//kernel::ControlFlow::~ControlFlow() {
//
//}


// ==================================
// Managing Running PCB:


void kernel::ControlFlow::setNextRunning() {
    this->running = this->scheduler->popNextRunning(*this);

    while (this->running->isDone()) {
        kernel::PCB::destroyPCB(this->running);
        this->running = this->scheduler->popNextRunning(*this);
    }
}

bool kernel::ControlFlow::onClockTick() {
    // mark one clock's tick on running's time
    ++this->elapsed_time;

    // mark one clock's tick on sleeping threads
    this->scheduler->onClockTick();

    // check if running's time is up now
    return this->elapsed_time >= this->running->getTimeSlice();
}


// ==================================
// Public Static Methods:


int kernel::ControlFlow::sleep(const time_t sleep_duration) {
    // get current running PCB and add to blocked queue
    kernel::PCB* const old_running = this->running;

    if (sleep_duration > 0)
        this->scheduler->pushSleep(old_running, sleep_duration);
    else
        this->scheduler->pushReady(old_running);

    // change current running to next ready in scheduler
    this->setNextRunning();

    // reset the elapsed time of the running PCB in yield
    //this->elapsed_time = 0UL;  // done in yield

    // yield:
    this->yield(old_running, this->running);

    return 0;  // success
}

void kernel::ControlFlow::yield() {
    kernel::RISCV::ABI_Function thread_dispatch(
        kernel::RISCV::ABI_Function::ID::THREAD_DISPATCH
    );

    thread_dispatch.call(this->processor);
}

void kernel::ControlFlow::yield(kernel::PCB *old_pcb, kernel::PCB *new_pcb) {
    // clear elapsed time here before `this` ptr gets lost
    this->elapsed_time = 0UL;

    if (old_pcb == new_pcb) return;

    //pushRegisters();

    kernel::PCB::contextSwitch(
        &old_pcb->context,
        &new_pcb->context
    );

    //popRegisters();
}

void kernel::ControlFlow::dispatch() {
    kernel::PCB *old_pcb = this->running;

    if (!old_pcb->is_done) {
        // push before popping next running
        this->scheduler->pushReady(old_pcb);
    } else {  // should not be destroyed by user ??
        kernel::PCB::destroyPCB(old_pcb);
    }

    //this->running = this->scheduler->popNextRunning(*this);
    this->setNextRunning();
    this->elapsed_time = 0UL;

    // do not store context of this function's local variables
    // as they are not being used after next instruction,
    // otherwise store it first and then restore it
    kernel::PCB::contextSwitch(&old_pcb->context,
                               &this->running->context);
}

void kernel::ControlFlow::mainWrapper() {  // still in the supervisor mode
    // self represents `this` ptr if this func were not static
    kernel::ControlFlow &self
        = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());

    self.setIsInInterruptRoutine(false);

    // leave supervisor mode and keep executing this function
    self.processor.sstatus().popSIEandSPP();

    kernel::PCB *running = self.running;
    running->main(running->main_arg);
    running->setIsDone(true);

    // once done give up the processor, fetch the instance again
    // instead of using `self` in order not to rely on stack ptr
    // since usr may have messed it up
    kernel::ControlFlow::getInstance(kernel::RISCV::getInstance()).yield();
}


// ==================================
// Managing Ready Threads:


void kernel::ControlFlow::terminateUserThreads() {
    // avoid any change of context on timer ticks
    //this->running = nullptr;

    this->do_ignore_user_threads_scheduler = true;

    // allow clock tick change of context again
    //this->running = this->kernel_main;
}

void kernel::ControlFlow::terminateKernelThreads() {
    kernel::ControlFlow::is_kernel_terminate_pending = true;
    kernel::ControlFlow::getInstance(kernel::RISCV::getInstance())
        .setRunning(nullptr);
}


// ==================================
// Managing Periodic Threads:


void kernel::ControlFlow::waitForPeriodicThreads() {
    for (bool do_not_quit = true; do_not_quit; ) {
        this->periodic_thread_counter_mutex->wait(*this);
        do_not_quit = this->periodic_thread_counter > 0;
        this->periodic_thread_counter_mutex->signal(*this);

        if (do_not_quit) this->yield();
    }
}

void kernel::ControlFlow::increasePeriodicThreadCount(::PeriodicThread *thread) {
//    this->periodic_thread_counter_mutex->wait(*this);

    ++this->periodic_thread_counter;
    this->periodic_threads->push_back(thread);

//    this->periodic_thread_counter_mutex->signal(*this);
}

void kernel::ControlFlow::decreasePeriodicThreadCount(::PeriodicThread *thread) {
//    this->periodic_thread_counter_mutex->wait(*this);

    --this->periodic_thread_counter;
    this->periodic_threads->removeValues(thread);

//    this->periodic_thread_counter_mutex->signal(*this);
}

void kernel::ControlFlow::signalKillOldestPeriodicThread() {
    this->periodic_thread_killer_sem->signal(*this);
}

void kernel::ControlFlow::lockPeriodicThread() {
    this->periodic_thread_counter_mutex->wait(*this);
}

void kernel::ControlFlow::unlockPeriodicThread() {
    this->periodic_thread_counter_mutex->signal(*this);
}


// ==================================
// Misc.


int kernel::ControlFlow::initKernelThreadsAndUserMode
    (kernel::PCB *kernel_main_pcb)
{
    // Create and set the Console Consumer Thread:

    thread_t console_consumer_ = nullptr;
    int err_code_consumer = thread_create(
        &console_consumer_,
        &kernel::ControlFlow::popCharsToConsoleController,
        nullptr
    );

    if (err_code_consumer < 0) {  // failed to create console_consumer
        return -2;
    }

    kernel::ControlFlow::setConsoleConsumer((kernel::PCB *) console_consumer_);


    // Create and set IDLE thread:

    thread_t idle = nullptr;
    int err_code_idle = thread_create(
        &idle, &kernel::ControlFlow::forEverLoop, nullptr
    );

    if (err_code_idle < 0) {  // failed to create IDLE
        return -3;
    }

    // set IDLE and its time slice to be 1 clock tick
    this->setIDLE((kernel::PCB *) idle);


    // Create and set Periodic Thread Killer Thread:

    thread_t periodic_thread_killer = nullptr;

    int err_code_periodic_thread_killer = thread_create(
        &periodic_thread_killer,
        &kernel::ControlFlow::killOldestPeriodicThread,
        nullptr
    );

    if (err_code_periodic_thread_killer < 0) {  // failed to create IDLE
        return -10;
    }

    // set IDLE and its time slice to be 1 clock tick
    this->setPeriodicThreadKiller((kernel::PCB *) periodic_thread_killer);


    // allow thread change when elapsed time reaches time slice
    this->running = this->kernel_main = kernel_main_pcb;

    return 0;  // success
}

time_t kernel::ControlFlow::getTimeLeft() const {
    return this->running->getTimeSlice() - this->elapsed_time;
}


// ==================================
// Getters:


bool kernel::ControlFlow::isInInterruptRoutine() const {
    return this->is_in_interrupt_routine;
}

kernel::PCB* kernel::ControlFlow::getRunning() const {
    return this->running;
}

time_t kernel::ControlFlow::getElapsedTime() const {
    return this->elapsed_time;
}

const kernel::PCB* kernel::ControlFlow::getKernelMain() const {
    return this->kernel_main;
}

const kernel::PCB* kernel::ControlFlow::getIDLE() const {
    return this->IDLE;
}

const kernel::PCB* kernel::ControlFlow::getConsoleConsumer() {
    return kernel::ControlFlow::console_consumer;
}

bool kernel::ControlFlow::doIgnoreUserThreads() const {
    return this->do_ignore_user_threads_scheduler;
}


// ==================================
// Setters:


void kernel::ControlFlow::setIsInInterruptRoutine(bool is_in_interrupt_routine) {
    this->is_in_interrupt_routine = is_in_interrupt_routine;
}

void kernel::ControlFlow::setRunning(kernel::PCB *running) {
    this->running = running;
}

void kernel::ControlFlow::setElapsedTime(time_t elapsed_time) {
    this->elapsed_time = elapsed_time;
}

void kernel::ControlFlow::setIDLE(kernel::PCB *idle) {
    this->IDLE = idle;
//    this->IDLE->setTimeSlice(1UL);
}

void kernel::ControlFlow::setConsoleConsumer(kernel::PCB *console_consumer_) {
    kernel::ControlFlow::console_consumer = console_consumer_;
}

void kernel::ControlFlow::terminateKernelThreads() {
    kernel::ControlFlow::is_kernel_terminate_pending = true;
    kernel::ControlFlow::getInstance(kernel::RISCV::getInstance())
        .setRunning(nullptr);
}

void kernel::ControlFlow::setPeriodicThreadKiller
    (kernel::PCB *periodic_thread_killer)
{
    this->periodic_thread_killer = periodic_thread_killer;
}


// ==================================
// Kernel Threads Bodies:


void kernel::ControlFlow::forEverLoop(void *) {
    for ( ; !kernel::ControlFlow::is_kernel_terminate_pending; ) {

    }
}

void kernel::ControlFlow::popCharsToConsoleController(void *) {
    auto &console = kernel::Console::getInstance();

    for ( ; !kernel::ControlFlow::is_kernel_terminate_pending; ) {
        int err_code = console.popOutputStreamCharToHardware(
            kernel::ControlFlow::getInstance(kernel::RISCV::getInstance())
        );

        // in case anything failed or write was
        // not allowed then yield
        if (err_code != 0) thread_dispatch();
    }
}

void kernel::ControlFlow::killOldestPeriodicThread(void *) {
    for ( ; !kernel::ControlFlow::is_kernel_terminate_pending; ) {
        auto &flow = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());
        flow.periodic_thread_killer_sem->wait(flow);

        // kill the oldest periodic thread created:
        flow.lockPeriodicThread();
        ::PeriodicThread *thread = flow.periodic_threads->getDataByIdx(0);
        flow.unlockPeriodicThread();

        thread->stopThread();
    }
}
