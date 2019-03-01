#!/bin/bash


rm -fr api
mkdir -p api/sys
for lib in gdi tts poll_event_api ipc_cmd tts key_api; do
    cp out/target/product/*/system/lib/lib${lib}.so api
done

for header in tts_api.h poll_event_api.h timer_api.h key_api.h ipc_cmd.h simple_draw.h; do
    cp $(find vendor/huaqin/packages/api/ -name ${header}) api
done

cp vendor/huaqin/packages/api/tts/main.c api/tts_demo_main.c
cp vendor/huaqin/packages/api/gdi/main.c api/gdi_demo_main.c

cat >api/sys/timerfd.h << EOF
/*
 * Copyright (C) 2013 The Android Open Source Project
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

#ifndef _SYS_TIMERFD_H_
#define _SYS_TIMERFD_H_

#include <fcntl.h> /* For O_CLOEXEC and O_NONBLOCK. */
#include <time.h>
#include <sys/types.h>

__BEGIN_DECLS

#define TFD_TIMER_ABSTIME (1 << 0)
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)

#define TFD_CLOEXEC O_CLOEXEC
#define TFD_NONBLOCK O_NONBLOCK

extern int timerfd_create(clockid_t, int);
extern int timerfd_settime(int, int, const struct itimerspec*,
                           struct itimerspec*);
extern int timerfd_gettime(int, struct itimerspec*);

__END_DECLS

#endif /* _SYS_TIMERFD_H */
EOF

cat > api/Makefile <<EOF
CC=~/bin/arm-linux-androideabi-4.7/bin/arm-linux-androideabi-gcc

all: tts_test gdi_test

gdi_test: gdi_demo_main.c
	\$(CC) -o \$@ \$< -std=c99 -O2 -Wall -I. -L. -lgdi
tts_test: tts_demo_main.c
	\$(CC) -o \$@ \$< -std=c99 -O2 -Wall -I. -L. -lipc_cmd -lkey_api -lpoll_event_api -ltts
clean:
	rm -f tts_test gdi_test
EOF
