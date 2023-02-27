#include "../h/PCB.hpp"
#include "../h/Scheduler.hpp"
#include "../h/ControlFlow.hpp"
#include "../h/RISCV.hpp"  // for displaySelf
#include "../h/Console.hpp"  // for displaySelf


// ==================================
// Constructor and Helpers:


kernel::Scheduler::Scheduler()
    : ready_queue(new kernel::utils::LinkedList<PCB *> ()),
      sleeping_queue(new kernel::utils::LinkedList<SleepDescriptor> ())
{

}

kernel::Scheduler& kernel::Scheduler::getInstance() {
    //  -fno-use-cxa-atexit
    static kernel::Scheduler instance;
    return instance;
}


// ==================================
// Destructor:


// must not destroy local static variable since
// project is missing __dso_handle
//kernel::Scheduler::~Scheduler() {
//    delete this->ready_PCB_queue;
//}


// ==================================
// Misc.


void kernel::Scheduler::onClockTick() {
    if (this->sleeping_queue->empty()) return;

    // mark one clock's tick
    auto cur = this->sleeping_queue->getNodeByIdx(0);
    --cur->data.sleep_left;

    while (cur && cur->data.sleep_left == 0) {
        this->ready_queue->push_back(cur->data.sleeping_pcb);
        cur = this->sleeping_queue->pop_front();  // set new head after popping
    }
}


// ==================================
// Public Getters:


bool kernel::Scheduler::empty() const {
    return this->ready_queue->empty();
}


// ==================================
// Public PCB Manipulation Methods:


void kernel::Scheduler::pushReady(kernel::PCB *to_add) {
    this->ready_queue->push_back(to_add);
}

void kernel::Scheduler::pushSleep(kernel::PCB *to_add, const time_t sleep_duration) {
    SleepDescriptor pcb_wrapper(to_add, sleep_duration);

    if (this->sleeping_queue->empty()) {  // to_add is the only one
        this->sleeping_queue->push_back(pcb_wrapper);
        return;
    }

    // preserve the sorted order by sleep duration
    // relative to first element in the list

    // check if this PCB will be new head:
    auto cur = this->sleeping_queue->getNodeByIdx(0);
    if ((int) (pcb_wrapper.sleep_left - cur->data.sleep_left) < 0) {  // PCB is new head
        cur->data.sleep_left -= sleep_duration;
        this->sleeping_queue->push_front(pcb_wrapper);
        return;
    } else {
        pcb_wrapper.sleep_left -= cur->data.sleep_left;
    }

    auto prev = cur;
    for ( cur = cur->next;
          cur && (int) (pcb_wrapper.sleep_left - cur->data.sleep_left) >= 0;
          prev = cur, cur = cur->next )
    {
        pcb_wrapper.sleep_left -= cur->data.sleep_left;
    }

    // insert after prev and before cur(if not at end)
    if (cur) cur->data.sleep_left -= pcb_wrapper.sleep_left;
    this->sleeping_queue->insertAfter(prev, pcb_wrapper);
}

kernel::PCB* kernel::Scheduler::popNextRunning(kernel::ControlFlow &flow) {
    if (this->ready_queue->empty()) return nullptr;

    kernel::PCB *next_PCB = nullptr;

    if (flow.doIgnoreUserThreads()) {  // skip all user threads
        // search for first non IDLE kernel thread and return it
        // if no such extract IDLE
        auto cur = this->ready_queue->getNodeByIdx(0);
        decltype(cur) prev = nullptr;
        decltype(cur) idle_prev = nullptr;
        for ( ; cur; prev = cur, cur = cur->next) {
            // mark IDLE to quickly extract if needed
            if (cur->data == flow.getIDLE()) {
                idle_prev = prev;
                continue;
            }

            if ( cur->data == flow.getKernelMain()
              || cur->data == flow.getConsoleConsumer() )
            {
                break;
            }
        }

        if (!cur) {  // only kernel thread is IDLE
            if (!idle_prev) {  // IDLE is head
                next_PCB = prev->data;
                this->ready_queue->pop_front();
            } else {
                next_PCB = idle_prev->next->data;
                delete this->ready_queue->extractNode(idle_prev->next, idle_prev);
            }
        } else {  // cur is first kernel thread
            next_PCB = cur->data;
            delete this->ready_queue->extractNode(cur, prev);
        }

    } else {  // user thread can be next running PCB
        // if IDLE is next push it to the end and return new
        // next (that way IDLE is returned only if it's the only ready PCB)
        if (this->ready_queue->getDataByIdx(0) == flow.getIDLE()) {
            this->ready_queue->appendNode
                    (this->ready_queue->extractByIdx(0));
        }

        next_PCB = this->ready_queue->getDataByIdx(0);
        this->ready_queue->pop_front();
    }

    return next_PCB;
}


// ==================================
// Logging for Debugging:


void kernel::Scheduler::displaySelf() const {
    auto &flow = kernel::ControlFlow::getInstance(kernel::RISCV::getInstance());
    auto &console = kernel::Console::getInstance();

    console.printString("Scheduler\n");

    console.printString("Running: ");
    console.printlnInt((size_t) flow.getRunning());

    // Log ready PCBs

    console.printString("ready queue of size: ");
    console.printlnInt(this->ready_queue->size());

    for (this->ready_queue->iter_begin();
         this->ready_queue->iter_not_end();
         this->ready_queue->iter_next())
    {
        if (*this->ready_queue->iter_data() == flow.getIDLE())
            console.printString("IDLE");
        else if (*this->ready_queue->iter_data() == flow.getConsoleConsumer())
            console.printString("ConsoleConsumer");
        else
            console.printInt((size_t) *this->ready_queue->iter_data());

        console.printString(", ");
    }

    // Log Sleeping PCBs

    console.printString("\nsleeping queue of size: ");
    console.printlnInt(this->sleeping_queue->size());

    for (this->sleeping_queue->iter_begin();
         this->sleeping_queue->iter_not_end();
         this->sleeping_queue->iter_next())
    {
        console.printString("(");
        console.printInt((size_t) this->sleeping_queue->iter_data()->sleeping_pcb);
        console.printString(", t: ");
        console.printInt((size_t) this->sleeping_queue->iter_data()->sleep_left);
        console.printString("), ");
    }

    console.printString("\nEnd of Scheduler\n");
}
