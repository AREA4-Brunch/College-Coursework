.global _ZN6kernel3PCB13contextSwitchEPNS0_7ContextES2_
.type _ZN6kernel3PCB13contextSwitchEPNS0_7ContextES2_, @function
.align 4
_ZN6kernel3PCB13contextSwitchEPNS0_7ContextES2_:
    // a0 - ptr to old->context
    // a1 - ptr to new->context
    sd ra, 0 * 8(a0)
    sd sp, 1 * 8(a0)

    ld ra, 0 * 8(a1)
    ld sp, 1 * 8(a1)

    ret
