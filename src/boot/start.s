.section .text
.global _start


_start:
    // Ensure we're in ARM mode
    mrs r0, cpsr
    bic r0, r0, #0x1F    // Clear mode bits
    orr r0, r0, #0x13    // Set SVC mode
    msr cpsr, r0
    // Set stack pointer
    ldr r0, =stack_top
    mov sp, r0

    //TODO: Zero out BSS

    bl main

_halt:
    b _halt
