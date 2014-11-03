#define SIG_SETMASK        2    /* for setting the signal mask */

#define UCONTEXT_MCONTEXT 20
#define UCONTEXT_SIGMASK  (UCONTEXT_MCONTEXT + 84)
#define UCONTEXT_REGSPACE (UCONTEXT_SIGMASK  +  4 + 31*4)
// note the +8 is to skip over the magic and size fields
// within vfp_sigframe to get directly to vfp_sigframe.user_vfp.fpregs[32]
#define UCONTEXT_FPREGS   (UCONTEXT_REGSPACE +  8)

#define MCONTEXT_ARM_R0   (UCONTEXT_MCONTEXT +  3*4)
#define MCONTEXT_ARM_SP   (MCONTEXT_ARM_R0   + 13*4)
#define MCONTEXT_ARM_LR   (MCONTEXT_ARM_R0   + 14*4)
#define MCONTEXT_ARM_CPSR (MCONTEXT_ARM_R0   + 16*4)

/*
struct ucontext {               // offset
uint32_t uc_flags;              //  0
struct ucontext *uc_link;       //  4  (+  4)
stack_t uc_stack;               //  8  (+  4)
mcontext_t uc_mcontext;         // 20  (+ 12)
sigset_t uc_sigmask;            // 104 (+ 84)
int _unused[31];                // 108 (+  4)
uint32_t uc_regspace[128];      // 232 (+ 31*4)
}

typedef struct sigcontext mcontext_t;
struct sigcontext {             // offset
long unsigned int trap_no;
long unsigned int error_code;
long unsigned int oldmask;
long unsigned int arm_r0;       // 12
// ...
long unsigned int arm_r10;
long unsigned int arm_fp;
long unsigned int arm_ip;
long unsigned int arm_sp;       // 64
long unsigned int arm_lr;       // 68
long unsigned int arm_pc;
long unsigned int arm_cpsr;
long unsigned int fault_address;
}
*/
