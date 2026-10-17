/**
 * @file coroutine/co_thread/co_thread.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the co_thread_t structure and related functions for stackful coroutine management.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common structure and functions used to create, manage, and switch stackful coroutines.
 */

#ifndef COROUTINE_CO_THREAD_H_
#define COROUTINE_CO_THREAD_H_

#include "coroutine/co_thread/context.h"
#include "coroutine/co_thread/status.h"
#include <stdlib.h>

// Macro to define the main co_thread context and the current co_thread pointer.
// Must be included in exactly one source file to define the main co_thread context and the current co_thread pointer.
#define CO_THREAD_IMPLEMENTATION co_thread_context_t main_co_thread_ctx; co_thread_t* curr_co_thread = nullptr;

#define CO_THREAD_DEFAULT_STACK_SIZE (2 * 1024) // 2 KB stack size for each coroutine


typedef struct {
  co_thread_context_t ctx;
  void* stack;
  void* (*func)(void*);
  void* arg;
  void* ret_val;
  size_t reserved_stack_sz;
  co_thread_status status;
} co_thread_t;

extern co_thread_context_t main_co_thread_ctx;
extern co_thread_t* curr_co_thread;

static inline void co_thread_entry_point(void) {
  if (curr_co_thread && curr_co_thread->func) {
    curr_co_thread->ret_val = curr_co_thread->func(curr_co_thread->arg);
  }

  if (curr_co_thread) {
    curr_co_thread->status = CO_THREAD_FINISHED;
    co_thread_switch(&curr_co_thread->ctx, &main_co_thread_ctx);
  }
}

static inline co_thread_t* co_thread_create_sz(void* (*func)(void*), void* arg, size_t stack_size) {
  co_thread_t* co = (co_thread_t*)malloc(sizeof(co_thread_t));
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
  co->status = CO_THREAD_READY;

  uintptr_t* stack_top = (uintptr_t*)((uintptr_t)co->stack + co->reserved_stack_sz);
  stack_top = (uintptr_t*)((uintptr_t)stack_top & -16L);

  *(--stack_top) = 0;
  *(--stack_top) = (uintptr_t)co_thread_entry_point;

  co->ctx.rsp = stack_top;
  co->ctx.rbp = stack_top;

  return co;
}

static inline co_thread_t* co_thread_create(void* (*func)(void*), void* arg) {
  return co_thread_create_sz(func, arg, CO_THREAD_DEFAULT_STACK_SIZE);
}

static inline void co_thread_yield(void) {
  co_thread_t* current_co = curr_co_thread;
  if (!current_co) {
    return;
  }

  current_co->status = CO_THREAD_SUSPENDED;
  co_thread_t* suspended_co = current_co;
  current_co = nullptr;

  co_thread_switch(&suspended_co->ctx, &main_co_thread_ctx);
}

static inline bool co_thread_resume(co_thread_t* co) {
  if (!co || co->status == CO_THREAD_FINISHED) {
    return false;
  }

  curr_co_thread = co;
  co->status = CO_THREAD_RUNNING;
  co_thread_switch(&main_co_thread_ctx, &co->ctx);

  return co->status != CO_THREAD_FINISHED;
}

static inline void co_thread_free(co_thread_t* co) {
  if (!co) {
    return;
  }

  if (co->stack) {
    free(co->stack);
  }
  free(co);
}

static inline size_t get_co_thread_reserved_stack_size(const co_thread_t* co) {
  if (!co) {
    return 0;
  }
  return co->reserved_stack_sz;
}

static inline size_t get_co_thread_stack_size(const co_thread_t* co) {
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

#endif // COROUTINE_CO_THREAD_H_