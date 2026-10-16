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

#define COROUTINE_DEFAULT_STACK_SIZE (64 * 1024) // 64 KB stack size for each coroutine

typedef struct {
  co_context_t ctx;
  void* stack;
  void* (*func)(void*);
  void* arg;
  void* ret_val;
  co_status status;
} coroutine_t;

static inline co_context_t* get_main_co_ctx() {
  static co_context_t main_ctx;
  return &main_ctx;
}

static inline coroutine_t* internal_curr_coroutine(bool set, coroutine_t* co) {
  static coroutine_t* curr_co = nullptr;
  if (set) {
    curr_co = co;
  }
  return curr_co;
}

static inline coroutine_t* get_curr_coroutine() {
  return internal_curr_coroutine(false, nullptr);
}

static inline void set_curr_coroutine(coroutine_t* co) {
  internal_curr_coroutine(true, co);
}

static inline void coroutine_entry_point(void) {
  coroutine_t* current_co = get_curr_coroutine();

  if (current_co && current_co->func) {
    current_co->ret_val = current_co->func(current_co->arg);
  }

  if (current_co) {
    current_co->status = CO_DEAD;
    co_switch(&current_co->ctx, get_main_co_ctx());
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
  co->status = CO_READY;

  uintptr_t* stack_top = (uintptr_t*)((uintptr_t)co->stack + stack_size);
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
  coroutine_t* current_co = get_curr_coroutine();
  if (!current_co) {
    return;
  }

  current_co->status = CO_SUSPENDED;
  coroutine_t *suspended_co = current_co;
  current_co = nullptr;

  co_switch(&suspended_co->ctx, get_main_co_ctx());
}

static inline bool co_resume(coroutine_t *co) {
  if (!co || co->status == CO_DEAD) {
    return false;
  }

  set_curr_coroutine(co);
  co->status = CO_RUNNING;
  co_switch(get_main_co_ctx(), &co->ctx);

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

#endif // COROUTINE_H_