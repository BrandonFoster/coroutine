/**
 * @file coroutine/co_context.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the co_context_t structure and co_switch function for coroutine context switching.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common structure and function used to save and restore coroutine contexts.
 */

#ifndef CO_CONTEXT_H_
#define CO_CONTEXT_H_

typedef struct {
  void* rsp; // stack pointer
  void* rbp; // base pointer
  void* rbx; // callee-saved register
  void* r12; // callee-saved register
  void* r13; // callee-saved register
  void* r14; // callee-saved register
  void* r15; // callee-saved register
} co_context_t;

__attribute__((naked, noinline, sysv_abi))
static void co_switch(co_context_t* save, co_context_t* restore) {
  __asm__ volatile(
    // save current context into 'save' structure (pointed to by %rcx)
    "movq %%rsp, 0(%0)\n\t"
    "movq %%rbp, 8(%0)\n\t"
    "movq %%rbx, 16(%0)\n\t"
    "movq %%r12, 24(%0)\n\t"
    "movq %%r13, 32(%0)\n\t"
    "movq %%r14, 40(%0)\n\t"
    "movq %%r15, 48(%0)\n\t"

    // restore target context from 'restore' structure (pointed to by %rdx)
    "movq 0(%1), %%rsp\n\t"
    "movq 8(%1), %%rbp\n\t"
    "movq 16(%1), %%rbx\n\t"
    "movq 24(%1), %%r12\n\t"
    "movq 32(%1), %%r13\n\t"
    "movq 40(%1), %%r14\n\t"
    "movq 48(%1), %%r15\n\t"
    "ret\n\t"
    :
    : "r" (save), "r" (restore)
    : "memory"
  );
}

#endif // CO_CONTEXT_H_