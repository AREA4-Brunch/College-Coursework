#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include "../h/LinkedList.hpp"
#include "../h/ControlFlow.hpp"


namespace kernel {


class PCB;


class Semaphore
{

 public:


    // ==================================
    // Constructors:


    Semaphore(unsigned int init=1) : counter(init) {}

    /**
     * Meant to be used only in supervisor mode where no
     * other allocation on allocator is requested at the same time.
     */
    void* operator new (size_t size);

    void* operator new(size_t size, void *memory);


    // ==================================
    // Destructor:


    /// Deblocks every queued thread and returns negative
    /// value in all their queued wait() calls.
    ~Semaphore();

    /**
     * Meant to be used only in supervisor mode where no
     * other deallocation on allocator is requested at same time.
     */
    void operator delete (void *data);


    // ==================================
    // Misc.


    /// Returns whether next call of wait(), without any signal()
    /// call in between, would block the thread that called it.
    bool wouldNextWait() const { return this->counter == 0; }


    // ==================================
    // Getters:


    /// Its absolute value represents number of blocked PCBs.
    int getValue() const;


    // ==================================
    // Methods:


    /// Returns 0 in case of success, else negative value.
    int wait(kernel::ControlFlow &flow);

    /// Returns 0 in case of success, else negative value.
    int signal(kernel::ControlFlow &flow);


 protected:


    /// Returns 0 in case of success, else negative value.
    int block(kernel::ControlFlow &flow);

    /// Returns 0 in case of success, else negative value.
    int unblock(kernel::ControlFlow &flow);



 private:


    // ==================================
    // Attributes:


    int counter = 1;
    kernel::utils::LinkedList<kernel::PCB *> queue_blocked;

};


} // kernel namespace


#endif //SEMAPHORE_HPP
