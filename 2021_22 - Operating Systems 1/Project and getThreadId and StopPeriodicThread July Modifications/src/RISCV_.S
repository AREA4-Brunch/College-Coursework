
.global _ZNK6kernel5RISCV13pushRegistersEv
.type _ZNK6kernel5RISCV13pushRegistersEv, @function
.align 4
_ZNK6kernel5RISCV13pushRegistersEv:
    // skips register 0 cause it is always ground, but reserve
    // space for it, it can useful for sth.
    // also skip ra and sp
    // stack grows lower -> higher; 256 = 8B * 32regs
    addi sp, sp, -256
    .irp index, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    sd x\index, \index * 8(sp)
    .endr
    ret


.global _ZNK6kernel5RISCV12popRegistersEv
.type _ZNK6kernel5RISCV12popRegistersEv, @function
.align 4
_ZNK6kernel5RISCV12popRegistersEv:
    // skips register 0 cause it is always ground, but reserve
    // space for it, it can useful for sth.
    // also skip ra and sp
    // stack grows lower -> higher; 256 = 8B * 32regs
    .irp index, 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256
    ret


/// not func, but macro because after popping ra
/// from the stack the func would not return where
/// it was called
.macro POP_REGISTERS_MACRO
    .irp index, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    ld x\index, \index * 8(sp)
    .endr
    // stack grows lower -> higher; 256 = 8B * 32regs
    addi sp, sp, 256
.endm


// no need for additional ret instruction this way
.macro PUSH_REGISTERS_MACRO
    addi sp, sp, -256
    .irp index, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    sd x\index, \index * 8(sp)
    .endr
.endm


// Just declare method representing body of
// interrupt routine which implements logic of
// interruption being described by scause.
.global _ZN6kernel5RISCV19processInterruptionEv


.global _ZN6kernel5RISCV11onInterruptEv
.type _ZN6kernel5RISCV11onInterruptEv, @function
.align 4
_ZN6kernel5RISCV11onInterruptEv:
    // push all registers on stack
    PUSH_REGISTERS_MACRO

    // store stack pointer into a0 which
    // is then used in processInterruption
    // mv a0, sp  // using fp instead

    // process the interruption in .cpp code
    call _ZN6kernel5RISCV19processInterruptionEv

    // move past the instruction that caused interrupt,
    // returning sepc and incrementing it to next instruction

    //csrr x6, sepc
    //addi x6, x6, 4  // increment sepc
    //csrw sepc, x6

    // overwrite value of register a0 on stack with
    // the return value of the interrupt routine
    // and restore it later with all other registers
    // sd a0, 10 * 8(sp)

    // pop all registers off stack:
    POP_REGISTERS_MACRO

    sret
