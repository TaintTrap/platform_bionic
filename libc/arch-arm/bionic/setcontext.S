#include <machine/asm.h>

#include "ucontext_i.h"

/* Set user context from information of variable pointed to by UCP. */
/* int setcontext (const ucontext_t *ucp) */

ENTRY(setcontext)
    mov	r4, r0 /* save *ucp since sigprocmask needs r0 */

    /* Restore VFPv3 registers d0 to d31 */
    add r0, #UCONTEXT_FPREGS
    vldmia r0!, {d0-d15}
    vldmia r0!, {d16-d31}
    /* Restore the floating-point status register. */
    ldr r1, [r0]
    vmsr fpscr, r1

    /* Now bring back the signal status. */
    /* int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) */
    mov	r0, #SIG_SETMASK
    add	r1, r4, #UCONTEXT_SIGMASK
    mov	r2, #0
    bl  PIC_SYM(_C_LABEL(sigprocmask), PLT)

    /* CPSR_f : nzcvq bits */
    /* q is for overflow in some DSP instructions */
    ldr r0, [r4, #MCONTEXT_ARM_CPSR]
    msr cpsr_f, r0

    /* Restore general purpose regs r0 to r15 from ucp */
    add r14, r4, #MCONTEXT_ARM_R0
    ldm r14, {r0-r15}
END(setcontext)
