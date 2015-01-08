/*
   Memory Leak Detection - Header
   Copyright (c) 2012-2015 Cheryl Natsu 
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   */

#ifndef _MEMLEAK_H_
#define _MEMLEAK_H_

/* initialize memory block list when 
 * malloc invoked at first time automaticly 
 * destroy memory block list and print memory leak report 
 * when program exit automaticly */
#define MEMLEAK_MODE_AUTO 0

/* initialize memory, print memory leak report
 * and destroy memory block list manually */
#define MEMLEAK_MODE_MANUAL 1

#define MEMLEAK_MODE_DEFAULT (MEMLEAK_MODE_AUTO)

/* change memory leak working mode */
int memleak_mode(int value);
/* initialize memory block list */
int memleak_init(void);
/* destroy memory block list */
void memleak_uninit(void);
/* manually print memory leak report */
void memleak_report(void);

#define malloc(x) memleak_malloc(x, __func__, __FILE__, __LINE__)
#define free(x) memleak_free(x, __func__, __FILE__, __LINE__)
#define calloc(x) memleak_calloc(x, __func__, __FILE__, __LINE__)
#define realloc(ptr, size) memleak_realloc(ptr, size, __func__, __FILE__, __LINE__)

#endif

