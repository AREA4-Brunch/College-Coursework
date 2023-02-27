#ifndef RISCV_HPP
#define RISCV_HPP


#include "../lib/hw.h"
#include "../h/ControlFlow.hpp"  // has function which is friend of ABI
#include "../h/syscall_c.h"


// ==================================================
// Reading Data from `a` Registers:
// won't affect any register other than destination


/// Reads data from a0 register into register used
/// for given destination variable.
#define KERNEL_RISCV_R_A0(destination) \
__asm__ volatile ("mv %[dest], a0" : [dest] "=r"((destination)));

/// Reads data from a1 register into register used
/// for given destination variable.
#define KERNEL_RISCV_R_A1(destination) \
__asm__ volatile ("mv %[dest], a1" : [dest] "=r"((destination)));

/// Reads data from a2 register into register used
/// for given destination variable.
#define KERNEL_RISCV_R_A2(destination) \
__asm__ volatile ("mv %[dest], a2" : [dest] "=r"((destination)));

/// Reads data from a3 register into register used
/// for given destination variable.
#define KERNEL_RISCV_R_A3(destination) \
__asm__ volatile ("mv %[dest], a3" : [dest] "=r"((destination)));

/// Reads data from a4 register into register used
/// for given destination variable.
#define KERNEL_RISCV_R_A4(destination) \
__asm__ volatile ("mv %[dest], a4" : [dest] "=r"((destination)));

/// Reads data from a5 register into register used
/// for given destination variable.
#define KERNEL_RISCV_R_A5(destination) \
__asm__ volatile ("mv %[dest], a5" : [dest] "=r"((destination)));

/// Reads data from a6 register into register used
/// for given destination variable.
#define KERNEL_RISCV_R_A6(destination) \
__asm__ volatile ("mv %[dest], a6" : [dest] "=r"((destination)));

/// Reads data from a7 register into register used
/// for given destination variable.
#define KERNEL_RISCV_R_A7(destination) \
__asm__ volatile ("mv %[dest], a7" : [dest] "=r"((destination)));


// ==================================================
// Writing Data to `a` Registers:
// could be inline funcs but staying persistant


/// Writes into a0 register the value from register
/// represented by given argument.
#define KERNEL_RISCV_W_A0(new_value) \
__asm__ volatile ("mv a0, %[new_val]" : : [new_val] "r"((new_value)));

/// Writes into a1 register the value from register
/// represented by given argument.
#define KERNEL_RISCV_W_A1(new_value) \
__asm__ volatile ("mv a1, %[new_val]" : : [new_val] "r"((new_value)));

/// Writes into a2 register the value from register
/// represented by given argument.
#define KERNEL_RISCV_W_A2(new_value) \
__asm__ volatile ("mv a2, %[new_val]" : : [new_val] "r"((new_value)));

/// Writes into a3 register the value from register
/// represented by given argument.
#define KERNEL_RISCV_W_A3(new_value) \
__asm__ volatile ("mv a3, %[new_val]" : : [new_val] "r"((new_value)));

/// Writes into a4 register the value from register
/// represented by given argument.
#define KERNEL_RISCV_W_A4(new_value) \
__asm__ volatile ("mv a4, %[new_val]" : : [new_val] "r"((new_value)));

/// Writes into a5 register the value from register
/// represented by given argument.
#define KERNEL_RISCV_W_A5(new_value) \
__asm__ volatile ("mv a5, %[new_val]" : : [new_val] "r"((new_value)));

/// Writes into a6 register the value from register
/// represented by given argument.
#define KERNEL_RISCV_W_A6(new_value) \
__asm__ volatile ("mv a6, %[new_val]" : : [new_val] "r"((new_value)));

/// Writes into a7 register the value from register
/// represented by given argument.
#define KERNEL_RISCV_W_A7(new_value) \
__asm__ volatile ("mv a7, %[new_val]" : : [new_val] "r"((new_value)));



namespace kernel {


class RISCV
{

 public:


    static uint64 constexpr INSTRUCTION_SIZE_IN_BYTES = 4UL;


    // ==================================================
    // Available ABI calls:


    /// ABI Function descriptor.
    struct ABI_Function
    {
        friend void* ::mem_alloc(size_t size);
        friend int ::mem_free(void* data);
        friend void kernel::ControlFlow::yield();  // thread_dispatch equivalent
        friend int ::thread_create ( thread_t* handle,
                                     void(*start_routine)(void*),
                                     void* arg );
        friend int ::thread_exit();
        friend void ::thread_dispatch();
        friend int ::sem_open(sem_t *handle, unsigned init);
        friend int ::sem_close(sem_t handle);
        friend int ::sem_wait(sem_t handle);
        friend int ::sem_signal(sem_t handle);
        friend int ::time_sleep(time_t sleep_duration);
        friend char ::getc();
        friend void ::putc(char chr);
        friend int ::getThreadId();
        friend void ::thread_kill(thread_t handle);

     public:

        /// Maximum number of argument that can be passed to
        /// an ABI call of kernel::RISCV processor.
        static constexpr unsigned int MAX_ARGS = 4U;

        /// ID/Operation Code of available ABI calls.
        enum class ID : uint64 {
            MALLOC = 0x01UL,
            FREE = 0x02UL,
            THREAD_CREATE = 0x11UL,
            THREAD_EXIT = 0x12UL,
            THREAD_DISPATCH = 0x13UL,
            SEMAPHORE_OPEN = 0x21UL,
            SEMAPHORE_CLOSE = 0x22UL,
            SEMAPHORE_WAIT = 0x23UL,
            SEMAPHORE_SIGNAL = 0x24UL,
            TIME_SLEEP = 0x31UL,
            GETC = 0x41UL,
            PUTC = 0x42UL,
            GET_THREAD_ID = 0x43UL,
            THREAD_KILL = 0x44UL
        };


        // ==================================
        // Constructor:

        /**
         * @param func_id - id/operation code recognized by ABI
         */
        explicit ABI_Function(const ID func_id) : func_id(func_id) {}


        // ==================================
        // Fields publicly available for change:

        // id/operation code recognized by ABI
        ID func_id;

        // each function argument is 64 bits (fits into 1 register)
        uint64 func_args[MAX_ARGS];


     private:

        /**
         * Evokes interrupt routine via ecall on given
         * processor and performs ABI call specified by
         * this function's id.
         * Passes operation code and arguments of
         * this function to that call.
         * Returns the return value of ABI if
         * it has any, otherwise returns operation code.
         *
         * @param processor - processor to execute
         *                    function on
         * @return Return value of ABI call from
         *         interrupt routine, in case
         *         this function has no return value
         *         returns operation code.
         */
        void* call(kernel::RISCV &processor) const;

    };


    // ==================================================
    // Registers Declarations:


    struct Register {
        virtual uint64 read() const = 0;
        virtual void write(uint64 new_reg_val) = 0;
    };

    struct Maskable {
        enum BitMask : uint64;

        /// Sets bits to 1 which given mask selects using 1s.
        virtual void mask_set(uint64 mask) = 0;

        /// Sets bits to 0 which given mask selects using 1s.
        virtual void mask_clear(uint64 mask) = 0;
    };

    struct scause_t : public Register
    {
        /// Returns value stored in scause register.
        /// Value should describe the reason for entering
        /// supervisor mode.
        uint64 read() const override;

        /// Stores given value in scause register.
        /// Value should describe the reason for entering
        /// supervisor mode.
        void write(const uint64 scause) override;

        enum Cause : uint64 {
            TIMER = 0x8000000000000001UL,
            OUTER_HARDWARE = 0x8000000000000009UL,  // e.g. keyboard
            ILLEGAL_INSTRUCTION = 0x0000000000000002UL,
            FORBIDDEN_ADDRESS_READ = 0x0000000000000005UL,
            FORBIDDEN_ADDRESS_WRITE = 0x0000000000000007UL,
            ECALL_USER_MODE = 0x0000000000000008UL,
            ECALL_SUPERVISOR_MODE = 0x0000000000000009UL
        };
    };

    struct sepc_t : public Register
    {
        /// Returns value stored in sepc register.
        /// Value should be value of program counter
        /// of user mode.
        uint64 read() const override;

        /// Stores given value in sepc register.
        /// Value should be value of program counter
        /// of user mode.
        void write(const uint64 sepc) override;

        /// Increments sepc by one RISCV instruction size (4).
        /// Is not atomic.
        void incrementInstruction();
    };

    struct stvec_t : public Register
    {
        /// Returns value stored in stvec register.
        /// Value should be address of interrupt routine
        /// and divisible by 4.
        uint64 read() const override;

        /// Stores given value in stvec register.
        /// Value should be address of interrupt routine
        /// and divisible by 4.
        void write(const uint64 stvec) override;
    };

    struct stval_t : public Register
    {
        /// Returns value stored in stval register.
        /// Supervisor trap value register.
        uint64 read() const override;

        /// Stores given value in stval register.
        /// Supervisor trap value register.
        void write(const uint64 stval) override;
    };

    struct sstatus_t : public Register, public Maskable
    {
        /// Returns value stored in sstatus register.
        /// Value should describe the interruption.
        uint64 read() const override;

        /// Stores given value in sstatus register.
        /// Value should describe the interruption.
        void write(const uint64 sstatus) override;

        void mask_set(const uint64 mask) override;

        void mask_clear(const uint64 mask) override;

        /**
         * !important must not be inline as it has to set
         * register ra before it is called
         *
         * Writes register ra into sepc and calls sret.
         */
        void popSIEandSPP() volatile;

        enum BitMask : uint64 {
            // mask interruptions only in supervisor
            // mode, otherwise enabled constantly
            // 0 - mask outer interruptions
            // 1 - allow outer interruptions
            SIE = ((uint64) 1 << 1),

            // previous value of SIE
            SPIE = ((uint64) 1 << 5),

            // which mode control flow came from,
            // 0 - user mode, 1 - supervisor mode
            SPP = ((uint64) 1 << 8),
        };
    };

    /// supervisory interrupt pending
    struct sip_t : public Register, public Maskable
    {
        /// Returns value stored in sip register.
        uint64 read() const override;

        /// Stores given value in sip register.
        void write(const uint64 sip) override;

        void mask_set(const uint64 mask) override;

        void mask_clear(const uint64 mask) override;

        enum BitMask : uint64 {
            // 1 sets a request for software interruption,
            // 0 means it has just ended
            SSIP = ((uint64) 1 << 1),

            //STIP = ((uint64) 1 << 5),

            // 1 - there is a request for outer hardware
            // interruption
            SEIP = ((uint64) 1 << 9),
        };
    };


 public:


    static RISCV& getInstance();

    // Forbid copies because of Singleton
    RISCV(const RISCV &) = delete;
    RISCV& operator= (const RISCV &) = delete;


    // ==================================================
    // Available Registers Getters:


    scause_t& scause()    { return this->scause_reg; }
    sepc_t& sepc()        { return this->sepc_reg; }
    stvec_t& stvec()      { return this->stvec_reg; }
    stval_t& stval()      { return this->stval_reg; }
    sstatus_t& sstatus()  { return this->sstatus_reg; }
    sip_t& sip()          { return this->sip_reg; }


    // ==================================================
    // Useful asm shortcuts:


    /**
     * Pushes registers a3...a31 onto the stack.
     * Does not push registers ra and sp.
     *
     * Implemented in RISCV.S file.
     */
    void pushRegisters() const;

    /**
     * Pops registers a3...a31 off the stack.
     * Does not pop registers ra and sp.
     *
     * Implemented in RISCV.S file.
     */
    void popRegisters() const;

    /**
     * Enables or disables interruptions and therefore provides
     * asynchronous and synchronous execution respectively.
     *
     * @param enforce_sync - if true interruptions will be blocked
     *                       and code will execute synchronously,
     *                       if false then opposite.
     */
    void setBlockInterruptions(const bool do_enforce_sync);

    /**
     * Sets SPP bit of sstatus register to 0 which represents
     * user mode.
     */
    void setUserMode(const bool into_usr_mode);


 private:


    // ==================================================
    // Constructor/Destructor and Singleton:


    RISCV();
    ~RISCV() = default;


    // ==================================================
    // Unique Registers:


    scause_t scause_reg;
    sepc_t sepc_reg;
    stvec_t stvec_reg;
    stval_t stval_reg;
    sstatus_t sstatus_reg;
    sip_t sip_reg;


    // ==================================================
    // Interrupt Routine:


    /**
     * Function written into the stvec register and
     * is evoked by processor when entering its own
     * interrupt routine.
     * Implemented in RISCV.S file.
     */
    static void onInterrupt();

    /**
     * Implements logic of any interruption, identified
     * by a0 register.
     * Should be called between storing and restoring
     * registers/old context.
     */
    static void processInterruption();

};


}  // kernel namespace



// ==================================================
// Implementation of inline methods:



// ==================================================
// scause_t Definition:


inline uint64 kernel::RISCV::scause_t::read() const {
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void kernel::RISCV::scause_t::write(const uint64 scause) {
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}


// ==================================================
// sepc_t Definition:


inline uint64 kernel::RISCV::sepc_t::read() const {
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void kernel::RISCV::sepc_t::write(const uint64 sepc) {
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

/// Increments sepc by one RISCV instruction size (4).
/// Is not atomic.
//inline void kernel::RISCV::sepc_t::incrementInstruction() {
//
//}


// ==================================================
// stvec_t Definition:


inline uint64 kernel::RISCV::stvec_t::read() const {
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void kernel::RISCV::stvec_t::write(const uint64 stvec) {
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}


// ==================================================
// stval_t Definition:


inline uint64 kernel::RISCV::stval_t::read() const {
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void kernel::RISCV::stval_t::write(const uint64 stval) {
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}


// ==================================================
// sstatus_t Definition:


inline uint64 kernel::RISCV::sstatus_t::read() const {
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void kernel::RISCV::sstatus_t::write(const uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline void kernel::RISCV::sstatus_t::mask_set(const uint64 mask) {
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void kernel::RISCV::sstatus_t::mask_clear(const uint64 mask) {
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}


// ==================================================
// sip_t Definition:


inline uint64 kernel::RISCV::sip_t::read() const {
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void kernel::RISCV::sip_t::write(const uint64 sip) {
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void kernel::RISCV::sip_t::mask_set(const uint64 mask) {
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void kernel::RISCV::sip_t::mask_clear(const uint64 mask) {
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}


#endif  // RISCV_HPP
