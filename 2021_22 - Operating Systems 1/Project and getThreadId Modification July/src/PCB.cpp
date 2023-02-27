#include "../h/PCB.hpp"
#include "../h/RISCV.hpp"
#include "../h/Scheduler.hpp"
#include "../h/ControlFlow.hpp"


int kernel::PCB::ID = 0;


// ==================================
// Constructors and Helpers:


kernel::PCB::PCB(main_t main, arg_t main_arg,
                 void *stack, time_t time_slice)
    : main(main),
      main_arg(main_arg),
      stack(reinterpret_cast<uint64*> (stack)),
      context({
          reinterpret_cast<uint64> (&kernel::ControlFlow::mainWrapper),
          reinterpret_cast<uint64> (stack)
      }),
      is_done(false),
      time_slice(time_slice)
{
    // add this PCB into scheduler via createPCB
    //kernel::Scheduler::getInstance().push(this);
}

kernel::PCB* kernel::PCB::createPCB(main_t process_main,
                                    arg_t main_arg,
                                    void *stack)
{
    auto &allocator = kernel::MemoryAllocator::getInstance();
    void *memory = allocator.mallocBytes(sizeof(PCB));
    if (!memory) return nullptr;
    kernel::PCB *new_PCB = new (memory) PCB(process_main,
                                             main_arg,
                                             stack,
                                             kernel::PCB::DEFAULT_TIME_SLICE );

    // done from interrupt routine instead
//    if (process_main != nullptr)  // if nullptr, main is currently executing
//        kernel::Scheduler::getInstance().pushReady(new_PCB);

    return new_PCB;
}

void* kernel::PCB::operator new(size_t size, void *memory) {
    return (PCB *) memory;
}


// ==================================
// Destructor:


kernel::PCB::~PCB() {
    auto &allocator = kernel::MemoryAllocator::getInstance();

    // release the stack if it was allocated
    if (this->stack) allocator.free(this->stack);
}

void kernel::PCB::destroyPCB(PCB *to_destroy)
{
    if (!to_destroy) return;

    // call destructor and free memory
    delete to_destroy;
}

void kernel::PCB::operator delete(void *data) {
    auto &allocator = kernel::MemoryAllocator::getInstance();
    // destructor is called implicitly before operator delete
    //((PCB *) data)->~PCB();
    allocator.free(data);
}


// ==================================
// Setters:


void kernel::PCB::setIsDone(const bool isDone) {
    this->is_done = isDone;
}
