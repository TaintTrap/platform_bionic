#include <machine/asm.h>

#include "ucontext_i.h"

/* Set user context from information of variable pointed to by UCP. */
/* int setcontext (const ucontext_t *ucp) */

ENTRY(setcontext)
	mov	r4, r0 /* save *ucp since sigprocmask needs r0 */

	/* Now bring back the signal status. */
  /* int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) */
	mov	r0, #SIG_SETMASK
	add	r1, r4, #UCONTEXT_SIGMASK
	mov	r2, #0
  bl  PIC_SYM(_C_LABEL(sigprocmask), PLT)

  /* Restore general purpose regs r0 to r15 from ucp */
	add   lr, r4, #MCONTEXT_ARM_R0
	ldmia lr, {r0-r15}          /* appears to work - deprecated? */
  /* No need to restore CPSR - the caller will set PC LSB 0/1 depending on mode */
END(setcontext)
