/**
 * @file coroutine/co_task/co_task.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the co_task_t structure and related functions for stackless coroutine management.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common structure and functions used to create, manage, and switch stackless coroutines.
 */

#ifndef COROUTINE_CO_TASK_H_
#define COROUTINE_CO_TASK_H_

#include "coroutine/co_task/status.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t state_line;
} co_task_t;

static inline void co_task_init(co_task_t *task) {
  if (!task) {
    return;
  }
  task->state_line = 0;
}

static inline void co_task_reset(co_task_t *task) {
  if (!task) {
    return;
  }
  task->state_line = 0;
}

#define co_task_begin(task_ptr) \
  switch ((task_ptr)->state_line) { \
  case 0:

#define co_task_yield(task_ptr) \
  do { \
    (task_ptr)->state_line = __LINE__; \
    return CO_TASK_YIELDED; \
    case __LINE__:; \
  } while (0)

#define co_task_await(task_ptr, sub_future_ptr) \
  do { \
    (task_ptr)->state_line = __LINE__; \
    case __LINE__: \
    if ((sub_future_ptr) == CO_TASK_PENDING || (sub_future_ptr) == CO_TASK_YIELDED) { \
      return CO_TASK_PENDING; \
    } \
  } while (0)

#define co_task_end(task_ptr) \
    (task_ptr)->state_line = UINT32_MAX; \
    return CO_TASK_COMPLETED; \
  } \
  return CO_TASK_COMPLETED;

#endif // COROUTINE_CO_TASK_H_