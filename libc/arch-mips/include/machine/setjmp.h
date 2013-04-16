/*
 * Copyright (C) 2012 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _ARCH_MIPS_MACHINE_SETJMP_H_
#define _ARCH_MIPS_MACHINE_SETJMP_H_

#define _JBLEN          157    /* size, in longs, of a jmp_buf */

#define SC_REGMASK      (0*REGSZ)
#define SC_STATUS       (1*REGSZ)
#define SC_PC           (2*REGSZ)
#define SC_REGS         (SC_PC+8)
#define SC_FPREGS       (SC_REGS+32*8)
#define SC_ACX          (SC_FPREGS+32*REGSZ_FP)
#define SC_FPC_CSR      (SC_ACX+1*REGSZ)
#define SC_FPC_EIR      (SC_ACX+2*REGSZ)
#define SC_USED_MATH    (SC_ACX+3*REGSZ)
#define SC_DSP          (SC_ACX+4*REGSZ)
#define SC_MDHI         (SC_ACX+5*REGSZ)
#define SC_MDLO         (SC_MDHI+8)
#define SC_HI1          (SC_MDLO+8)
#define SC_LO1          (SC_HI1+1*REGSZ)
#define SC_HI2          (SC_HI1+2*REGSZ)
#define SC_LO2          (SC_HI1+3*REGSZ)
#define SC_HI3          (SC_HI1+4*REGSZ)
#define SC_LO3          (SC_HI1+5*REGSZ)
/* OpenBSD compatibility */
#define SC_MASK         SC_REGMASK
#define SC_FPUSED       SC_USED_MATH

#endif /* !_ARCH_MIPS_INCLUDE_MACHINE_SETJMP_H_ */
