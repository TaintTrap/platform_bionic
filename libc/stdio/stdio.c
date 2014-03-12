/*	$OpenBSD: stdio.c,v 1.9 2005/08/08 08:05:36 espie Exp $ */
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

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "local.h"

/*
 * Small standard I/O/seek/close functions.
 * These maintain the `known seek offset' for seek optimisation.
 */
int
__sread(void *cookie, char *buf, int n)
{
	FILE *fp = cookie;
	int ret;

#ifdef WITH_TAINT_TRACKING
  int fd = fp->_file;
  char path[1024];
  char result[1024];

  sprintf(path, "/proc/self/fd/%d", fd);
  memset(result, 0, sizeof(result));
  readlink(path, result, sizeof(result)-1);

  int xbuf;
  int xtag = TAINT_CLEAR;
  int xret = fgetxattr(fd, TAINT_XATTR_NAME, &xbuf, sizeof(xbuf));

  if (xret > 0) {
      xtag = xbuf;
      fprintf(stdout, "%s :  read(%d) taint tag: 0x%x\n", __func__, fd, xtag);
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

	ret = read(fp->_file, buf, n);
	/* if the read succeeded, update the current offset */
	if (ret >= 0) {
#ifdef WITH_TAINT_TRACKING
      fprintf(stderr, "%s :  read(%d) %s buf: %p n: %4d offset: %6ld ret: %4d\n", __func__, fd, result, buf, n, fp->_offset, ret);
    if (xtag) {
        if (ret != n) {
            fprintf(stderr, "%s :  read(%d) incomplete n: %4d ret: %4d\n", __func__, fd, n, ret);
        }
        emu_set_taint_array((uint32_t)buf + (uint32_t)fp->_offset, xtag, ret);
        // emu_protect_mem();
    }
#endif
		fp->_offset += ret;
  }	else {
      fp->_flags &= ~__SOFF;	/* paranoia */
  }
	return (ret);
}

int
__swrite(void *cookie, const char *buf, int n)
{
	FILE *fp = cookie;

	if (fp->_flags & __SAPP)
		(void) lseek(fp->_file, (off_t)0, SEEK_END);
	fp->_flags &= ~__SOFF;	/* in case FAPPEND mode is set */

#ifdef WITH_TAINT_TRACKING
  int fd = fp->_file;

  // avoid emu write deadlock by skipping special fd's. ignore cases:
  // 1) stdin (0), stdout(1) and stderr(2)
  // 2) emu writes to trace file
  // 3) emu was never used yet (only reads can set taint + init emu)
  if (fd > 2 && fd != emu_get_trace_fd() && emu_initialized()) {
      char path[1024];
      char result[1024];
      sprintf(path, "/proc/self/fd/%d", fd);
      memset(result, 0, sizeof(result));
      readlink(path, result, sizeof(result)-1);

      fprintf(stderr, "%s: write(%d) %s buf: %p n: %4d offset: %6ld\n", __func__, fd, result, buf, n, fp->_offset);

      // TODO: what about file offset? doesn't seem to be updated/used on writes?
      uint32_t xtag = emu_get_taint_array((uint32_t)buf, (uint32_t)n);

      if (xtag) {
          // TODO: append new tag to original tag instead of overwritting it
          int xret = fsetxattr(fd, TAINT_XATTR_NAME, &xtag, sizeof(xtag), 0);
          // check if setting taint was successful
          if (xret < 0) {
              if (errno == ENOSPC || errno == EDQUOT) {
                  fprintf(stderr, "TaintLog: fsetxattr(%d): not enough room to set xattr\n", fd);
              } else if (errno == ENOTSUP) {
                  /* XATTRs are not supported. No need to spam the logs */
              } else if (errno == EPERM) {
                  /* Strange interaction with /dev/log/main. Suppress the log */
              } else {
                  fprintf(stderr,"TaintLog: fsetxattr(%d): unknown error code %d\n", fd, errno);
              }
          }
      }
  }
#endif

	return (write(fp->_file, buf, n));
}

fpos_t
__sseek(void *cookie, fpos_t offset, int whence)
{
	FILE *fp = cookie;
	off_t ret;

	ret = lseek(fp->_file, (off_t)offset, whence);
	if (ret == (off_t)-1)
		fp->_flags &= ~__SOFF;
	else {
		fp->_flags |= __SOFF;
		fp->_offset = ret;
	}
	return (ret);
}

int
__sclose(void *cookie)
{
  FILE *fp = cookie;
#ifdef WITH_TAINT_TRACKING
  int fd = fp->_file;
  char path[1024];
  char result[1024];

  sprintf(path, "/proc/self/fd/%d", fd);
  memset(result, 0, sizeof(result));
  readlink(path, result, sizeof(result)-1);

  fprintf(stderr, "%s: close(%d) %s\n", __func__, fd, result);
#endif
	return (close(fp->_file));
}
