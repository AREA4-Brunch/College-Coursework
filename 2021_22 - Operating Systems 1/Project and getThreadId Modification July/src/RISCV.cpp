#include "../h/RISCV.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/PCB.hpp"
#include "../h/ControlFlow.hpp"
#include "../h/Semaphore.hpp"
#include "../h/Console.hpp"


// ==================================
// Constructor and Helpers:


kernel::RISCV::RISCV() {
    // store the address of interrupt routine for
    // this processor to use
    this->stvec_reg.write((uint64) RISCV::onInterrupt);

    // disable interruptions at start:
    this->setBlockInterruptions(true);
}

kernel::RISCV& kernel::RISCV::getInstance() {
    static kernel::RISCV instance;
    return instance;
}


// ==================================
// ABI Function Descriptor:


void *kernel::RISCV::ABI_Function::call(kernel::RISCV &processor) const
{
    uint64 volatile tmp_reg;

    // describe ABI call with all register values of
    // given processor, push value onto stack as you
    // load them and then unload into appropriate registers
    __asm__ volatile ("addi sp, sp, -64");  // 64 == 8B * 8regs

    // operation id/code
    tmp_reg = (uint64) this->func_id;
    __asm__ volatile ("sd %[src], 0 * 8(sp)" : : [src] "r" (tmp_reg));

    // pass all possible ABI function args to stack
    tmp_reg = this->func_args[0];
    __asm__ volatile ("sd %[src], 1 * 8(sp)" : : [src] "r" (tmp_reg));

    tmp_reg = this->func_args[1];
    __asm__ volatile ("sd %[src], 2 * 8(sp)" : : [src] "r" (tmp_reg));

    tmp_reg = this->func_args[2];
    __asm__ volatile ("sd %[src], 3 * 8(sp)" : : [src] "r" (tmp_reg));

    tmp_reg = this->func_args[3];
    __asm__ volatile ("sd %[src], 4 * 8(sp)" : : [src] "r" (tmp_reg));


    // pass ABI function args from the stack into a0, a1, ...
    // of given processor
    __asm__ volatile ("ld a0, 0 * 8(sp)");
    __asm__ volatile ("ld a1, 1 * 8(sp)");
    __asm__ volatile ("ld a2, 2 * 8(sp)");
    __asm__ volatile ("ld a3, 3 * 8(sp)");
    __asm__ volatile ("ld a4, 4 * 8(sp)");


    // pop off the stack
    __asm__ volatile ("addi sp, sp, 64");  // 64 == 8B * 8regs

    // call the ABI on given processor and return
    // the result of the operation
    __asm__ volatile ("ecall");
    KERNEL_RISCV_R_A0(tmp_reg);  // fetch return value

    return reinterpret_cast<void *> (tmp_reg);
}


// ==================================
// Public Registers Setters:


void kernel::RISCV::sstatus_t::popSIEandSPP() volatile {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void kernel::RISCV::sepc_t::incrementInstruction() {
    uint64 volatile sepc = this->read()
                         + kernel::RISCV::INSTRUCTION_SIZE_IN_BYTES;
    this->write(sepc);
}

void kernel::RISCV::setBlockInterruptions(const bool do_enforce_sync) {
    if (do_enforce_sync)  // sync
        this->sstatus_reg.mask_clear(sstatus_t::BitMask::SIE);
    else  // async
        this->sstatus_reg.mask_set(sstatus_t::BitMask::SIE);
}

void kernel::RISCV::setUserMode(const bool into_usr_mode) {
    if (into_usr_mode)
        this->sstatus_reg.mask_clear(sstatus_t::BitMask::SPP);
    else
        this->sstatus_reg.mask_set(sstatus_t::BitMask::SPP);
}


// ==================================
// Private Interrupt Routine Methods:


void kernel::RISCV::processInterruption() {
    // ptr replacing `this`, should not be static func if
    // multiple processors
    kernel::RISCV * const self = &kernel::RISCV::getInstance();
    // get scause register
    const uint64 volatile scause = self->scause_reg.read();

    // set user mode the very first time interrupts are called
    // from supervisor mode
    if (scause == kernel::RISCV::scause_t::Cause::ECALL_SUPERVISOR_MODE) {
        self->setUserMode(true);
    }

    kernel::ControlFlow &flow = kernel::ControlFlow::getInstance(*self);
    const bool prev_is_in_interrupt_routine = flow.isInInterruptRoutine();
    flow.setIsInInterruptRoutine(true);

    // process interruptions not caused by ecall here:
    if ( scause != kernel::RISCV::scause_t::Cause::ECALL_USER_MODE
      && scause != kernel::RISCV::scause_t::Cause::ECALL_SUPERVISOR_MODE )
    {   // did not get here via ecall

        switch (scause)
        {
            case kernel::RISCV::scause_t::Cause::TIMER: {
                if (!flow.getRunning()) {  // running has not yet been set
                    self->sip_reg.mask_clear(sip_t::BitMask::SSIP);
                    break;
                }

                if (flow.onClockTick()) {
                    // running PCB is now supposed to give up control of processor

                    // push sepc and sstatus of the running process to stack
                    // local variables are set by compiler to be stored on stack
                    uint64 volatile sepc = self->sepc_reg.read();
                    uint64 volatile sstatus = self->sstatus_reg.read();

                    // reset the elapsed time of the running PCB
                    // kernel::PCB::elapsed_time = 0UL;  // done inside dispatch()

                    flow.dispatch();  // modifies stack pointer

                    flow.setIsInInterruptRoutine(true);

                    // restore the sepc and sstatus of the new running process
                    self->sepc_reg.write(sepc);
                    self->sstatus_reg.write(sstatus);
                }

                self->sip_reg.mask_clear(sip_t::BitMask::SSIP);
                break;
            }

            case kernel::RISCV::scause_t::Cause::OUTER_HARDWARE: {
                // cause is e.g. console

//                kernel::Console::getInstance().printStringFromInterruptRoutine(
//                "OUTER_HARDWARE\n"
//                );

                int interrupt_hw_code = plic_claim();

                switch (interrupt_hw_code) {
                    case ::CONSOLE_IRQ: {  // interrupt from console
                        uint64 volatile sepc = self->sepc_reg.read();
                        uint64 volatile sstatus = self->sstatus_reg.read();

                        kernel::Console::getInstance().onInterrupt(flow);

                        flow.setIsInInterruptRoutine(true);
                        self->sstatus_reg.write(sstatus);
                        self->sepc_reg.write(sepc);

                        break;
                    }

                    default:
                        break;
                }

                // notify appropriate hardware that interrupt request
                // has been processed
                plic_complete(interrupt_hw_code);

                break;
            }

            case kernel::RISCV::scause_t::Cause::ILLEGAL_INSTRUCTION: {
                kernel::Console::getInstance().printString(
                        "ILLEGAL_INSTRUCTION\n"
                );
                break;
            }

            case kernel::RISCV::scause_t::Cause::FORBIDDEN_ADDRESS_READ: {
                kernel::Console::getInstance().printString(
                    "FORBIDDEN_ADDRESS_READ\n"
                );

                break;
            }

            case kernel::RISCV::scause_t::Cause::FORBIDDEN_ADDRESS_WRITE: {
                kernel::Console::getInstance().printString(
                        "FORBIDDEN_ADDRESS_WRITE\n"
                );
                break;
            }

            default: {
                kernel::Console::getInstance().printString(
                        "UNRECOGNIZED SCAUSE\n"
                );
                break;
            }
        }

        flow.setIsInInterruptRoutine(prev_is_in_interrupt_routine);

        return;
    }

    // else: scause == ECALL_USER_MODE || scause == ECALL_SUPERVISOR_MODE

    // get the id of the interrupt reason stored in
    // register a0 on stack in case of software interrupt
    uint64 volatile interrupt_id;
    __asm__ volatile (
          "ld %[dest], 10 * 8(fp)"
        : [dest] "=r" (interrupt_id)
    );

    // value to return by this ABI, when no
    // return value it is just operation code being restored
    uint64 volatile interrupt_retval = interrupt_id;

    // Perform appropriate ABI action based on ABI call ID
    switch ((kernel::RISCV::ABI_Function::ID) interrupt_id)
    {
        case kernel::RISCV::ABI_Function::ID::MALLOC: {
            // void* mem_alloc (size_t size);

            // get ABI call arguments
            uint64 volatile arg1;
            __asm__ volatile (
                  "ld %[dest], 11 * 8(fp)"
                : [dest] "=r" (arg1)
            );

            // execute the ABI call and store its return value
            auto &allocator = kernel::MemoryAllocator::getInstance();

            // get return value(address) which is void* as uint64
            interrupt_retval = reinterpret_cast<uint64>
                                (allocator.malloc(arg1));

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            // move one instruction after ecall
            self->sepc_reg.incrementInstruction();

            break;
        }

        case kernel::RISCV::ABI_Function::ID::FREE: {
            // int mem_free (void*);

            // get ABI call arguments
            uint64 volatile arg1;
            __asm__ volatile (
                  "ld %[dest], 11 * 8(fp)"
                : [dest] "=r" (arg1)
            );

            // execute the ABI call and store its return value
            auto &allocator = kernel::MemoryAllocator::getInstance();

            interrupt_retval = allocator.free(reinterpret_cast<void *> (arg1));

            // in case of success set return value to 0
            if (static_cast<int> (interrupt_retval) > 0)
                interrupt_retval = 0;

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            // move one instruction after ecall
            self->sepc_reg.incrementInstruction();

            break;
        }

        case kernel::RISCV::ABI_Function::ID::THREAD_CREATE: {
//            class _thread;
//            typedef _thread* thread_t;
//            int thread_create (
//                thread_t* handle,
//                void(*start_routine)(void*),
//                void* arg,
//                void* stack_space
//            )

            // get ABI call arguments
            uint64 volatile handle;
            __asm__ volatile (
                  "ld %[dest], 11 * 8(fp)"
                : [dest] "=r" (handle)
            );

            uint64 volatile start_routine;
            __asm__ volatile (
                  "ld %[dest], 12 * 8(fp)"
                : [dest] "=r" (start_routine)
            );

            uint64 volatile arg;
            __asm__ volatile (
                  "ld %[dest], 13 * 8(fp)"
                : [dest] "=r" (arg)
            );

            uint64 volatile stack_space;
            __asm__ volatile (
                  "ld %[dest], 14 * 8(fp)"
                : [dest] "=r" (stack_space)
            );

            kernel::PCB *new_PCB = kernel::PCB::createPCB(
                reinterpret_cast<void (*) (void*)>(start_routine),
                reinterpret_cast<void *> (arg),
                reinterpret_cast<void *> (stack_space)
            );

            if (!new_PCB) {  // failed to create thread
                interrupt_retval = -1;  // failure code
            } else {  // successfully created PCB
                // if body/main of new_thread is not nullptr then it
                // does not represent currently running thread and
                // therefore should be in scheduler
                if (new_PCB->getMain() != nullptr) {
                    kernel::Scheduler::getInstance().pushReady(new_PCB);
                } else {
                    //flow.setRunning(new_PCB);
                }

                // modify the handle to point to constructed new thread
                __asm__ volatile (
                      "sd %[data], (%[dest])"
                    :
                    : [data] "r" (new_PCB),
                      [dest] "r" (handle)
                );

                interrupt_retval = 0;  // success code
            }

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            // move one instruction after ecall
            self->sepc_reg.incrementInstruction();

            break;
        }

        case kernel::RISCV::ABI_Function::ID::THREAD_EXIT: {
            // int thread_exit ();
            kernel::PCB *running = flow.getRunning();

            if (!running) {  // there is no thread to exit
                interrupt_retval = -1;  // error code

                // store return value in register a0 on stack that
                // will load back once the interrupt routine is done
                // if no return value it is just operation code being restored
                __asm__ volatile (
                      "sd %[data], 10 * 8(fp)"
                    :
                    : [data] "r" (interrupt_retval)
                      //[sp] "r" (SP)
                    : "cc"
                );

                // move one instruction after ecall
                self->sepc_reg.incrementInstruction();

                break;
            }

            // pick up running thread and label as done
            running->setIsDone(true);

            goto thread_dispatch_label;

            // break;
        }

        case kernel::RISCV::ABI_Function::ID::THREAD_DISPATCH: {
        thread_dispatch_label:
            // void thread_dispatch ();
            // running PCB is now supposed to give up control of processor

            // push sepc and sstatus of the running process to stack
            // local variables are set by compiler to be stored on stack
            // sepc is supposed move past ecall, but the new running
            // thread might not as it may have been stopped by timer
            // so sepc has to be increased before dispatch
            uint64 volatile sepc = self->sepc_reg.read()
                                 + kernel::RISCV::INSTRUCTION_SIZE_IN_BYTES;
            uint64 volatile sstatus = self->sstatus_reg.read();

            // reset the elapsed time of the running PCB
            // kernel::PCB::elapsed_time = 0UL;  // done inside dispatch()

            flow.dispatch();  // modifies stack pointer

            flow.setIsInInterruptRoutine(true);

            // restore the sepc and sstatus of the new running process
            self->sepc_reg.write(sepc);
            self->sstatus_reg.write(sstatus);

            break;  // skip sepc increment, also setting up return value
        }

        case kernel::RISCV::ABI_Function::ID::SEMAPHORE_OPEN: {
//            class _sem;
//            typedef _sem* sem_t;
//            int sem_open ( sem_t* handle, unsigned init );

            // get ABI call arguments
            uint64 volatile handle;
            __asm__ volatile (
                    "ld %[dest], 11 * 8(fp)"
                    : [dest] "=r" (handle)
            );

            uint64 volatile init;
            __asm__ volatile (
                    "ld %[dest], 12 * 8(fp)"
                    : [dest] "=r" (init)
            );

            kernel::Semaphore *semaphore = new kernel::Semaphore(init);
            if (!semaphore) {  // malloc failed
                interrupt_retval = -1;
            } else {  // successfully created semaphore
                // modify the handle to point to constructed new semaphore
                __asm__ volatile (
                    "sd %[src], (%[dest])"
                   :
                   : [src] "r" (semaphore),
                     [dest] "r" (handle)
                );

                interrupt_retval = 0;
            }

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            // move one instruction after ecall
            self->sepc_reg.incrementInstruction();

            break;
        }

        case kernel::RISCV::ABI_Function::ID::SEMAPHORE_CLOSE: {
//            class _sem;
//            typedef _sem* sem_t;
//            int sem_close (sem_t handle);

            // get ABI call arguments
            uint64 volatile handle;
            __asm__ volatile (
                  "ld %[dest], 11 * 8(fp)"
                : [dest] "=r" (handle)
            );

            kernel::Semaphore *semaphore
                = reinterpret_cast<kernel::Semaphore *> (handle);

            delete semaphore;

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            // move one instruction after ecall
            self->sepc_reg.incrementInstruction();

            break;
        }

        case kernel::RISCV::ABI_Function::ID::SEMAPHORE_WAIT: {
//            class _sem;
//            typedef _sem* sem_t;
//            int sem_wait (sem_t id);

            // get ABI call arguments
            uint64 volatile id;
            __asm__ volatile (
                "ld %[dest], 11 * 8(fp)"
                : [dest] "=r"(id)
            );

            kernel::Semaphore * const semaphore = (Semaphore *) id;

            // push sepc and sstatus of the running process to stack
            // local variables are set by compiler to be stored on stack
            // sepc is supposed move past ecall, but the new running
            // thread might not as it may have been stopped by timer
            // so sepc has to be increased before dispatch
            uint64 volatile sepc = self->sepc_reg.read()
                                 + kernel::RISCV::INSTRUCTION_SIZE_IN_BYTES;
            uint64 volatile sstatus = self->sstatus_reg.read();

            // in case there was no thread dispatch sepc was increased
            // by one instruction either way
            interrupt_retval = semaphore->wait(flow);

            flow.setIsInInterruptRoutine(true);

            // restore the sepc and sstatus of the new running process
            self->sepc_reg.write(sepc);
            self->sstatus_reg.write(sstatus);

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            // in case there was no thread dispatch sepc was increased
            // by one instruction either way
            // move one instruction after ecall
            //self->sepc_reg.incrementInstruction();

            break;
        }

        case kernel::RISCV::ABI_Function::ID::SEMAPHORE_SIGNAL: {
//            class _sem;
//            typedef _sem* sem_t;
//            int sem_signal (sem_t id);

            // get ABI call arguments
            uint64 volatile handle;
            __asm__ volatile (
                "ld %[dest], 11 * 8(fp)"
                : [dest] "=r" (handle)
            );

            kernel::Semaphore *semaphore = (kernel::Semaphore*) handle;

            interrupt_retval = semaphore->signal(flow);

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            // move one instruction after ecall
            self->sepc_reg.incrementInstruction();

            break;
        }

        case kernel::RISCV::ABI_Function::ID::TIME_SLEEP: {
//            typedef unsigned long time_t;
//            int time_sleep (time_t sleep_duration);

            // get ABI call arguments
            uint64 volatile sleep_duration;
            __asm__ volatile (
                  "ld %[dest], 11 * 8(fp)"
                : [dest] "=r" (sleep_duration)
            );

            // if no sleep then do not reset elapsed_time and just return
//            if (sleep_duration == 0) {
//                interrupt_retval = 0;  // success
//                break;
//            }

            // push sepc and sstatus of the running process to stack
            // local variables are set by compiler to be stored on stack
            // sepc is supposed move past ecall, but the new running
            // thread might not as it may have been stopped by timer
            // so sepc has to be increased before dispatch
            uint64 volatile sepc = self->sepc_reg.read()
                                 + kernel::RISCV::INSTRUCTION_SIZE_IN_BYTES;
            uint64 volatile sstatus = self->sstatus_reg.read();

            interrupt_retval = flow.sleep(sleep_duration);

            flow.setIsInInterruptRoutine(true);

            // restore the sepc and sstatus of the new running process
            self->sepc_reg.write(sepc);
            self->sstatus_reg.write(sstatus);

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            break;  // skip sepc increment
        }

        case kernel::RISCV::ABI_Function::ID::GETC: {

            // because of possible thread dispatch increment sepc
            // now and not later:
            // push sepc and sstatus of the running process to stack
            // local variables are set by compiler to be stored on stack
            // sepc is supposed move past ecall, but the new running
            // thread might not as it may have been stopped by timer
            // so sepc has to be increased before dispatch
            uint64 volatile sepc = self->sepc_reg.read()
                                 + kernel::RISCV::INSTRUCTION_SIZE_IN_BYTES;
            uint64 volatile sstatus = self->sstatus_reg.read();

            auto &console = kernel::Console::getInstance();

            int err_code = 0;
            const char read_chr = console.popFromInputStream(err_code);

            if (err_code < 0) {  // failed to retrieve a char
                interrupt_retval = EOF;
            }
            else {  // retrieved char successfully
                interrupt_retval = static_cast<uint64> (read_chr);
            }

            flow.setIsInInterruptRoutine(true);

            // restore the sepc and sstatus of the new running process
            self->sepc_reg.write(sepc);
            self->sstatus_reg.write(sstatus);

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            // already incremented sepc so do not
            // move one instruction after ecall
            // self->sepc_reg.incrementInstruction();

            break;
        }

        case kernel::RISCV::ABI_Function::ID::PUTC: {
//            void putc(char chr);

            // get ABI call arguments
            uint64 volatile chr;
            __asm__ volatile (
                  "ld %[dest], 11 * 8(fp)"
                : [dest] "=r" (chr)
            );

            // because of possible thread dispatch increment sepc
            // now and not later:
            // push sepc and sstatus of the running process to stack
            // local variables are set by compiler to be stored on stack
            // sepc is supposed move past ecall, but the new running
            // thread might not as it may have been stopped by timer
            // so sepc has to be increased before dispatch
            uint64 volatile sepc = self->sepc_reg.read()
                                 + kernel::RISCV::INSTRUCTION_SIZE_IN_BYTES;
            uint64 volatile sstatus = self->sstatus_reg.read();

            auto &console = kernel::Console::getInstance();
            console.pushIntoOutputStream((char) chr);

            flow.setIsInInterruptRoutine(true);

            // restore the sepc and sstatus of the new running process
            self->sepc_reg.write(sepc);
            self->sstatus_reg.write(sstatus);

            // no return value to store here

            // in case there was no thread dispatch sepc was increased
            // by one instruction either way
            // move one instruction after ecall
            //self->sepc_reg.incrementInstruction();

            break;
        }

        case kernel::RISCV::ABI_Function::ID::GET_THREAD_ID: {
            // int getThreadID();
            kernel::PCB *running = flow.getRunning();

            if (!running) {  // there is no running thread ?
                interrupt_retval = -1;  // error code

                // store return value in register a0 on stack that
                // will load back once the interrupt routine is done
                // if no return value it is just operation code being restored
                __asm__ volatile (
                      "sd %[data], 10 * 8(fp)"
                    :
                    : [data] "r" (interrupt_retval)
                      //[sp] "r" (SP)
                    : "cc"
                );

                // move one instruction after ecall
                self->sepc_reg.incrementInstruction();

                break;
            }

            interrupt_retval = running->getID();

            // perform a dispatch:

            // push sepc and sstatus of the running process to stack
            // local variables are set by compiler to be stored on stack
            // sepc is supposed move past ecall, but the new running
            // thread might not as it may have been stopped by timer
            // so sepc has to be increased before dispatch
            uint64 volatile sepc = self->sepc_reg.read()
                                 + kernel::RISCV::INSTRUCTION_SIZE_IN_BYTES;
            uint64 volatile sstatus = self->sstatus_reg.read();

            flow.dispatch();

            // in case there was no thread dispatch sepc was increased
            // by one instruction either way

            flow.setIsInInterruptRoutine(true);

            // restore the sepc and sstatus of the new running process
            self->sepc_reg.write(sepc);
            self->sstatus_reg.write(sstatus);

            // store return value in register a0 on stack that
            // will load back once the interrupt routine is done
            // if no return value it is just operation code being restored
            __asm__ volatile (
                  "sd %[data], 10 * 8(fp)"
                :
                : [data] "r" (interrupt_retval)
                  //[sp] "r" (SP)
                : "cc"
            );

            break;
        }

        default: {  // Unrecognized ABI call ID
            //interrupt_retval = 0;

            // move one instruction after ecall
            self->sepc_reg.incrementInstruction();

            break;
        }
    }

    flow.setIsInInterruptRoutine(prev_is_in_interrupt_routine);

    // set end of software interruption, done via sret automatically
//    self->sip_reg.mask_clear((uint64) kernel::RISCV::sip_t::BitMask::SSIP);

    return;
}
