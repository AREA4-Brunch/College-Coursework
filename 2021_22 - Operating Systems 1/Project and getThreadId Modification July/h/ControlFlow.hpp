#ifndef CONTROL_FLOW_HPP
#define CONTROL_FLOW_HPP


#include "../h/Scheduler.hpp"


namespace kernel {


class RISCV;
class PCB;


class ControlFlow
{

 public:


    static ControlFlow& getInstance(kernel::RISCV &processor);

    // Forbid copies because of Singleton:
    ControlFlow(const ControlFlow&) = delete;
    ControlFlow& operator= (const ControlFlow&) = delete;


    // ==================================
    // Control Flow of every thread:


    static void mainWrapper();


    // ==================================
    // Managing Running PCB:


    /// Pops next ready PCB from scheduler and sets it
    /// as new running.
    void setNextRunning();

    /// First increments elapsed_time by 1 and calls
    /// onClockTick on scheduler, then
    /// returns: elapsed_time >= running's time_slice.
    bool onClockTick();

    /**
     * Removes currently running PCB and reschedules it on
     * some processor and modifies given pointer to point
     * to that processor.
     */
    void dispatch();

    /// Moves currently running process into scheduler.
    /// Equivalent to thread_dispatch from C API.
    void yield();

    /// Performs context switch from old PCB to new PCB.
    void yield(kernel::PCB *old_pcb, kernel::PCB *new_pcb);

    /// Puts currently running thread to sleep.
    /// Once its awaken returns.
    /// Returns 0 in case of success, else negative value.
    int sleep(const time_t sleep_duration);


    // ==================================
    // Managing Ready Threads:


    /// Forbids scheduler to give the processor to
    /// any threads other then kernel threads including
    /// the one calling this function.
    void terminateUserThreads();


    // ==================================
    // Misc.


    /// Creates threads for console consumer and
    /// IDLE and uses ecall for that, therefore
    /// enables user mode. Afterwards set given thread
    /// as kernel main thread.
    /// Returns 0 in case of success, negative value otherwise.
    int initKernelThreadsAndUserMode(kernel::PCB *kernel_main_pcb);

    /// Returns number of clock ticks current running process has
    /// left until it gets put back into scheduler.
    time_t getTimeLeft() const;


    // ==================================
    // Getters:


    static const kernel::PCB* getConsoleConsumer();

    static bool isKernelTerminatePending() {
        return kernel::ControlFlow::is_kernel_terminate_pending;
    }

    bool isInInterruptRoutine() const;

    /// Returns pointer to PCB of the process currently
    /// being executed.
    kernel::PCB* getRunning() const;

    /// Returns number of ticks the process gets on
    /// processor before it is put back into scheduler.
    time_t getElapsedTime() const;

    /// Returns pointer to PCB of the kernel's
    /// int main() process.
    const kernel::PCB* getKernelMain() const;

    /// Returns pointer to PCB of the process being
    /// executed only when there are no other processes
    /// in the scheduler.
    const kernel::PCB* getIDLE() const;

    /// Should scheduler ignore user threads.
    bool doIgnoreUserThreads() const;


    // ==================================
    // Setters:


    void setIsInInterruptRoutine(bool is_in_interrupt_routine);

    /// Sets flag for kernel threads to naturally exit
    /// forever loop in case it reaches processor in time.
    static void terminateKernelThreads();


 private:


    // ==================================
    // Constructor, Destructor and Singleton:


    ControlFlow(kernel::RISCV &processor, kernel::Scheduler &scheduler);
//    ~ControlFlow();


    // ==================================
    // Attributes:


    /// kernel process that consumes chars from kernel::Console
    /// by popping them into the hardware, runs only when there
    /// are chars in kernel::Console
    static kernel::PCB *console_consumer;

    /// true if kernel is shutting down
    static bool is_kernel_terminate_pending;

    kernel::RISCV &processor;

    // pointer because destructor of static variables is not supported
    kernel::Scheduler *scheduler = nullptr;

    bool is_in_interrupt_routine = false;

    /// PCB of currently running process
    kernel::PCB *running = nullptr;

    /// number of ticks the running process has been using
    /// the processor for since last rescheduling
    time_t elapsed_time = 0UL;

    /// kernel process that represents int main()
    kernel::PCB *kernel_main = nullptr;

    /// kernel process that busy waits, runs only when there
    /// are no other PCBs in scheduler
    kernel::PCB *IDLE = nullptr;

    bool do_ignore_user_threads_scheduler = false;


    // ==================================
    // Private Setters:


    static void setConsoleConsumer(kernel::PCB *consoleConsumer);

    /// Sets pointer to PCB of the process currently
    /// being executed.
    void setRunning(kernel::PCB *running);

    /// Sets number of clock ticks the running process has been using
    /// the processor for since last rescheduling
    void setElapsedTime(time_t elapsedTime);

    /// Sets pointer to PCB of the process being
    /// executed only when there are no other processes
    /// in the scheduler.
    void setIDLE(kernel::PCB *idle);


    // ==================================
    // Kernel Threads Bodies:


    /// Body for IDLE.
    static void forEverLoop(void *);

    /// Body for console consumer thread.
    static void popCharsToConsoleController(void *);

};


}  // kernel namespace


#endif //CONTROL_FLOW_HPP
