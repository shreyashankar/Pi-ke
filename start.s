.globl _start
_start:
    mov r0, #0xD2
    msr cpsr_c, r0
    mov sp, #0x8000  @ interrupt stack pointer
    mov r0, #0xD3
    msr cpsr_c, r0      
    mov sp,#0x8000000 @ svc mode stack pointer
    bl _cstart
hang: b hang

