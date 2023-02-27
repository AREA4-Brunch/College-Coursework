#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP



#include "LinkedList.hpp"


namespace kernel {


class ControlFlow;
class PCB;


/// Queue of PCBs waiting to get the processor.
class Scheduler final
{

 public:


    static Scheduler& getInstance();

    // Forbid copies because of Singleton
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator= (const Scheduler&) = delete;


    // ==================================
    // Getters:


    /// Returns true if there are no PCBs in this
    /// scheduler, otherwise returns nullptr.
    bool empty() const;


    // ==================================
    // Misc.


    /// Updates sleeping time of sleeping threads
    /// and adds the ones whose time is now 0 into
    /// the ready PCBs queue.
    void onClockTick();


    // ==================================
    // Adding PCBs:


    /**
     * Adds given PCB into queue of ready processes.
     * Not thread-safe.
     *
     * @param to_add - pointer to ready PCB to add.
     */
    void pushReady(kernel::PCB *to_add);

    /**
     * Adds given PCB into queue of sleeping processes.
     * Not thread-safe.
     *
     * @param to_add - pointer to PCB to add to sleep queue.
     */
    void pushSleep(kernel::PCB *to_add, const time_t sleep_duration);


    // ==================================
    // Removing PCBs:


    /**
     * Removes PCB that is supposed to get the processor
     * next and returns it.
     * If that PCB is PCB::IDLE it puts it last and
     * returns new first element.
     * Not thread-safe.
     *
     * @return pointer to PCB that waited the most.
     */
    kernel::PCB* popNextRunning(kernel::ControlFlow &flow);


    // ==================================
    // Logging for Debugging:


    void displaySelf() const;


 private:


    struct SleepDescriptor {
        // time left until being put into ready queue relative
        // to time of the PCB right before it in the queue
        time_t sleep_left;
        kernel::PCB *sleeping_pcb;

        SleepDescriptor(kernel::PCB *sleeping_pcb, time_t sleep_left)
            : sleep_left(sleep_left), sleeping_pcb(sleeping_pcb) {}

        bool operator== (const kernel::Scheduler::SleepDescriptor &other) const {
            return this->sleeping_pcb == other.sleeping_pcb;
        }

        bool operator< (const kernel::Scheduler::SleepDescriptor &other) const {
            return this->sleep_left < other.sleep_left;
        }
    };


    // ==================================
    // Constructor and Singleton:


    Scheduler();

    // must not destroy local static variable since
    // project is missing __dso_handle
    //~Scheduler();


    // ==================================
    // Attributes:


    // must be pointer so there is no non-primitive's destructor as
    // __dso_handle missing error occurs
    kernel::utils::LinkedList<kernel::PCB *> *ready_queue;

    // must be pointer so there is no non-primitive's destructor as
    // __dso_handle missing error occurs
    kernel::utils::LinkedList<SleepDescriptor> *sleeping_queue;

};


}  // kernel namespace

#endif //SCHEDULER_HPP
