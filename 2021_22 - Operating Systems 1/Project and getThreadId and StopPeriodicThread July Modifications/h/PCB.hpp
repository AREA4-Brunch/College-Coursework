#ifndef PCB_HPP
#define PCB_HPP


#include "../lib/hw.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/Semaphore.hpp"


namespace kernel {


// Friend Declarations:
class ControlFlow;



/// Process Control Block.
class PCB final
{

    friend kernel::ControlFlow;


 public:

    /// Body/block of code which represents
    /// process to run with given argument.
    using arg_t = void *;
    using main_t = void (*) (arg_t);


    static uint64 const DEFAULT_STACK_SIZE = ::DEFAULT_STACK_SIZE;
    static uint64 const DEFAULT_TIME_SLICE = ::DEFAULT_TIME_SLICE;


    // ==================================
    // Constructors:


    explicit PCB(main_t main,
                 arg_t main_arg,
                 void *stack=nullptr,
                 time_t time_slice=DEFAULT_TIME_SLICE);

    /**
     * Allocates new PCB using MemoryAllocator, with
     * given function treated as its: void main(void *arg).
     *
     * @param process_main - pointer to void (*) () function
     *                       representing main() of the PCB
     * @param arg - argument passed to process_main
     * @return pointer to newly constructed PCB, in
     *         case of failure nullptr
     */
    static PCB* createPCB(main_t process_main,
                          arg_t main_arg,
                          void *stack=nullptr);

    void* operator new(size_t size, void *memory);


    // ==================================
    // Destructor:


    ~PCB();

    static void destroyPCB(PCB *to_destroy);

    /**
     * Meant to be used only in supervisor mode where no
     * other deallocation on allocator is requested at same time.
     */
    void operator delete (void *data);


    // ==================================
    // Getters:


    /// Returns whether this process is done executing.
    bool isDone() const { return this->is_done; }

    bool getWasSemaphoreClosedWhileWaiting() const {
        return this->was_semaphore_closed_while_waiting;
    }

    /// Returns number of ticks process gets before it is put back
    /// into scheduler again
    time_t getTimeSlice() const { return this->time_slice; }

    /// Returns body/main of this PCB.
    main_t getMain() const { return this->main; }

    /// Returns argument passed to body/main of this PCB.
    arg_t getArg() const { return this->main_arg; }

    int getID() const { return this->id; }


    // ==================================
    // Setters:


    /// Sets whether this process is done executing and then
    /// waits on `semaphore_done`.
    void setIsDone(const bool isDone);

    void setWasSemaphoreClosedWhileWaiting(bool wasSemaphoreClosedWhileWaiting) {
        this->was_semaphore_closed_while_waiting = wasSemaphoreClosedWhileWaiting;
    }


 private:


    /// Context of the PCB, not the process itself.
    struct Context
    {
        Context(uint64 ra, uint64 sp) : ra(ra), sp(sp) {}

        uint64 volatile ra = 0ULL;  /// return address after context switch

        /// ptr to last added register on the process's user stack
        uint64 volatile sp = 0ULL;
    };


    // ==================================
    // Attributes:

    static int ID;
    const int id = ++ID;

    main_t main;  /// main function of this PCB's process
    arg_t main_arg;  /// argument passed to main

    /// Beginning of stack the process/main treats as user stack.
    /// Also used by this PCB to push process's context
    /// during context switch.
    uint64 *stack = nullptr;

    Context context;

    bool is_done = false;  /// is process/main done executing

    /// number of ticks process gets before it is put back
    /// into scheduler again
    time_t time_slice = 0ULL;

    bool was_semaphore_closed_while_waiting = false;


    // ==================================
    // Private Getters:


    Context& getContext() { return this->context; }


    // ==================================
    // Private Setters:


    /// Sets number of ticks the process gets on
    /// processor before it is put back into scheduler.
    void setTimeSlice(time_t timeSlice) { time_slice = timeSlice; }


    // ==================================
    // Context Switch Helpers:


    /**
     * Stores registers ra and sp into old context and loads
     * those values from new context into processor's registers
     * and continues execution from new context's ra.
     *
     * Implemented in PCB_.S
     *
     * @param old_context
     * @param new_context
     */
    static void contextSwitch(Context *old_context,
                              Context *new_context);


};


}  // kernel namespace

#endif //PCB_HPP
