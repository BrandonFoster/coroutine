/**
 * @file coroutine/co_scheduler/worker_node.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the co_worker_node_t structure for scheduler worker nodes.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common structure used to represent scheduler worker nodes.
 */

#ifndef COROUTINE_CO_SCHEDULER_WORKER_NODE_H_
#define COROUTINE_CO_SCHEDULER_WORKER_NODE_H_

#include "coroutine/co_scheduler/worker_type.h"
#include "coroutine/co_thread/co_thread.h"
#include "coroutine/co_task/co_task.h"

typedef co_task_status (*co_task_func_t)(void* ctx);

typedef struct co_worker_node co_worker_node_t;
struct co_worker_node {
  co_worker_type type;
  union {
    co_thread_t* thread;
    struct {
      void* ctx;
      co_task_func_t func;
    } task;
  } worker;
  co_worker_node_t* next;
};

#endif // COROUTINE_CO_SCHEDULER_WORKER_NODE_H_