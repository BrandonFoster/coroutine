/**
 * @file coroutine/co_scheduler/co_scheduler.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the co_scheduler_t structure and functions for managing scheduler workers.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common structure and functions used to manage scheduler workers.
 */

#ifndef COROUTINE_CO_SCHEDULER_H_
#define COROUTINE_CO_SCHEDULER_H_

#include "coroutine/co_scheduler/worker_node.h"

typedef struct {
  co_worker_node_t* head;
  co_worker_node_t* tail;
  size_t pending_workers;
} co_scheduler_t;

static inline void co_scheduler_init(co_scheduler_t* scheduler) {
  scheduler->head = nullptr;
  scheduler->tail = nullptr;
  scheduler->pending_workers = 0;
}

static inline void co_scheduler_spawn_thread(co_scheduler_t* scheduler, co_worker_node_t* worker, co_thread_t* thread) {
  worker->type = CO_THREAD_WORKER;
  worker->worker.thread = thread;
  worker->next = nullptr;

  if (!scheduler->tail) {
    scheduler->head = scheduler->tail = worker;
  } else {
    scheduler->tail->next = worker;
    scheduler->tail = worker;
  }
  scheduler->pending_workers++;
}

static inline void co_scheduler_spawn_task(co_scheduler_t* scheduler, co_worker_node_t* worker, void* ctx, co_task_func_t func) {
  worker->type = CO_TASK_WORKER;
  worker->worker.task.ctx = ctx;
  worker->worker.task.func = func;
  worker->next = nullptr;

  if (!scheduler->tail) {
    scheduler->head = scheduler->tail = worker;
  } else {
    scheduler->tail->next = worker;
    scheduler->tail = worker;
  }
  scheduler->pending_workers++;
}

static inline bool co_scheduler_step(co_scheduler_t* scheduler) {
  if (!scheduler->head) return false;

  co_worker_node_t* curr_worker = scheduler->head;
  scheduler->head = curr_worker->next;
  if (!scheduler->head) {
    scheduler->tail = nullptr;
  }
  curr_worker->next = nullptr;

  bool finished = false;

  if (curr_worker->type == CO_THREAD_WORKER) {
    co_thread_resume(curr_worker->worker.thread);
    if (curr_worker->worker.thread->status == CO_THREAD_FINISHED) {
      finished = true;
    } else {
      co_scheduler_spawn_thread(scheduler, curr_worker, curr_worker->worker.thread);
    }
  } else if (curr_worker->type == CO_TASK_WORKER) {
    co_task_status status = curr_worker->worker.task.func(curr_worker->worker.task.ctx);
    if (status == CO_TASK_COMPLETED) {
      finished = true;
    } else {
      co_scheduler_spawn_task(scheduler, curr_worker, curr_worker->worker.task.ctx, curr_worker->worker.task.func);
    }
  }

  if (finished) {
    scheduler->pending_workers--;
  }
  
  return scheduler->pending_workers > 0;
}

static inline void co_scheduler_run(co_scheduler_t* scheduler) {
  while (co_scheduler_step(scheduler));
}

#endif // COROUTINE_CO_SCHEDULER_H_