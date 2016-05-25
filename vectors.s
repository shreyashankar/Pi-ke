.globl _vectors
.globl _vectors_end
        
_vectors:
  ldr pc, _reset_asm
  ldr pc, _undefined_instruction_asm
  ldr pc, _software_interrupt_asm
  ldr pc, _prefetch_abort_asm
  ldr pc, _data_abort_asm
  ldr pc, _reset_asm
  ldr pc, _interrupt_asm
@ The fast interrupt vector comes last so that 
@ it can just directly execute. So just make it
@ the impossible vector and interrupts jump here.
fast_interrupt_asm:
impossible_asm:
  sub   lr, lr, #4
  push  {lr}
  push  {r0-r12}
  mov   r0, lr                 @ Pass old pc
  bl    impossible_vector      @ C function
  pop   {r0-r12}
  ldm   sp!, {pc}^

_reset_asm:                   .word impossible_asm
_undefined_instruction_asm:   .word impossible_asm
_software_interrupt_asm:      .word impossible_asm
_prefetch_abort_asm:          .word impossible_asm
_data_abort_asm:              .word impossible_asm
_interrupt_asm:               .word interrupt_asm

_vectors_end:

interrupt_asm:
  sub   lr, lr, #4
  push  {lr}
  push  {r0-r12}
  mov   r0, lr              @ Pass old pc
  bl    interrupt_vector    @ C function
  pop   {r0-r12}
  ldm   sp!, {pc}^


