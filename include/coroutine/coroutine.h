/**
 * @file coroutine/coroutine.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the coroutine_t structure and related functions for coroutine management.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common structure and functions used to create, manage, and switch coroutines.
 */

#ifndef COROUTINE_H_
#define COROUTINE_H_

#include "coroutine/co_context.h"
#include "coroutine/co_status.h"
#include <stdlib.h>

// Macro to define the main coroutine context and the current coroutine pointer.
// Must be included in exactly one source file to define the main coroutine context and the current coroutine pointer.
#define COROUTINE_IMPLEMENTATION co_context_t main_co_ctx; coroutine_t* curr_co = nullptr;

#define COROUTINE_DEFAULT_STACK_SIZE (64 * 1024) // 64 KB stack size for each coroutine


typedef struct {
  co_context_t ctx;
  void* stack;
  void* (*func)(void*);
  void* arg;
  void* ret_val;
  size_t reserved_stack_sz;
  co_status status;
} coroutine_t;

extern co_context_t main_co_ctx;
extern coroutine_t* curr_co;

static inline void coroutine_entry_point(void) {
  if (curr_co && curr_co->func) {
    curr_co->ret_val = curr_co->func(curr_co->arg);
  }

  if (curr_co) {
    curr_co->status = CO_DEAD;
    co_switch(&curr_co->ctx, &main_co_ctx);
  }
}

static inline coroutine_t* co_create_sz(void* (*func)(void*), void* arg, size_t stack_size) {
  coroutine_t* co = (coroutine_t*)malloc(sizeof(coroutine_t));
  if (!co) {
    return nullptr;
  }

  co->stack = malloc(stack_size);
  if (!co->stack) {
    free(co);
    return nullptr;
  }

  co->func = func;
  co->arg = arg;
  co->ret_val = nullptr;
  co->reserved_stack_sz = stack_size;
  co->status = CO_READY;

  uintptr_t* stack_top = (uintptr_t*)((uintptr_t)co->stack + co->reserved_stack_sz);
  stack_top = (uintptr_t*)((uintptr_t)stack_top & -16L);

  *(--stack_top) = 0;
  *(--stack_top) = (uintptr_t)coroutine_entry_point;

  co->ctx.rsp = stack_top;
  co->ctx.rbp = stack_top;

  return co;
}

static inline coroutine_t *co_create(void* (*func)(void*), void* arg) {
  return co_create_sz(func, arg, COROUTINE_DEFAULT_STACK_SIZE);
}

static inline void co_yield(void) {
  coroutine_t* current_co = curr_co;
  if (!current_co) {
    return;
  }

  current_co->status = CO_SUSPENDED;
  coroutine_t *suspended_co = current_co;
  current_co = nullptr;

  co_switch(&suspended_co->ctx, &main_co_ctx);
}

static inline bool co_resume(coroutine_t *co) {
  if (!co || co->status == CO_DEAD) {
    return false;
  }

  curr_co = co;
  co->status = CO_RUNNING;
  co_switch(&main_co_ctx, &co->ctx);

  return co->status != CO_DEAD;
}

static inline void co_free(coroutine_t *co) {
  if (!co) {
    return;
  }

  if (co->stack) {
    free(co->stack);
  }
  free(co);
}

static inline size_t get_co_reserved_stack_size(const coroutine_t *co) {
  if (!co) {
    return 0;
  }
  return co->reserved_stack_sz;
}

static inline size_t get_co_stack_size(const coroutine_t *co) {
    if (!co) {
        return 0;
    }

    uintptr_t stack_top = (uintptr_t)co->stack + co->reserved_stack_sz;
    stack_top = stack_top & -16L;

    uintptr_t suspended_rsp = (uintptr_t)co->ctx.rsp;

    if (suspended_rsp > stack_top || suspended_rsp < (uintptr_t)co->stack) {
        return 0; 
    }

    return (size_t)(stack_top - suspended_rsp);
}

#endif // COROUTINE_H_