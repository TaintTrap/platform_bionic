/*	$OpenBSD: fwrite.c,v 1.5 2005/08/08 08:05:36 espie Exp $ */
/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include "local.h"
#include "fvwrite.h"

/*
 * Write `count' objects (each size `size') from memory to the given file.
 * Return the number of whole objects written.
 */
size_t
fwrite(const void *buf, size_t size, size_t count, FILE *fp)
{
	size_t n;
	struct __suio uio;
	struct __siov iov;
	int ret;

	iov.iov_base = (void *)buf;
	uio.uio_resid = iov.iov_len = n = count * size;
	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;

#ifdef WITH_TAINT_TRACKING
  int fd = fp->_file;
  char path[1024];
  char result[1024];

  sprintf(path, "/proc/self/fd/%d", fd);
  memset(result, 0, sizeof(result));
  readlink(path, result, sizeof(result)-1);

  fprintf(stderr, "%s  : write(%d) %s buf: %p n: %4d offset: %6ld\n", __func__, fd, result, buf, n, fp->_offset);

  int xbuf;
  int xtag = TAINT_CLEAR;
  int xret = fgetxattr(fd, TAINT_XATTR_NAME, &xbuf, sizeof(xbuf));

  if (xret > 0) {
      xtag = xbuf;
      // fprintf(stdout, "%s  : write(%d) taint tag: 0x%x\n", __func__, fd, xtag);
  } else {
      if (errno == ENOATTR) {
          // fprintf(stdout, "fgetxattr(%s): no taint tag\n", result);
      } else if (errno == ERANGE) {
          fprintf(stderr, "TaintLog: fgetxattr(%s) contents to large\n", result);
      } else if (errno == ENOTSUP) {
          /* XATTRs are not supported. No need to spam the logs */
      } else if (errno == EPERM) {
          /* Strange interaction with /dev/log/main. Suppress the log */
      } else {
          fprintf(stderr, "TaintLog: fgetxattr(%s): unknown error code %d\n", result, errno);
      }
  }
#endif

	/*
	 * The usual case is success (__sfvwrite returns 0);
	 * skip the divide if this happens, since divides are
	 * generally slow and since this occurs whenever size==0.
	 */
	FLOCKFILE(fp);
	ret = __sfvwrite(fp, &uio);
	FUNLOCKFILE(fp);
	if (ret == 0)
		return (count);
	return ((n - uio.uio_resid) / size);
}
