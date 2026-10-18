/**
 * @file coroutine/co_scheduler/worker_type.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the co_worker_type enumeration for scheduler worker types.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common enumeration used to represent scheduler worker types.
 */

#ifndef COROUTINE_CO_SCHEDULER_WORKER_TYPE_H_
#define COROUTINE_CO_SCHEDULER_WORKER_TYPE_H_

typedef enum {
  CO_TASK_WORKER,
  CO_THREAD_WORKER
} co_worker_type;

#endif // COROUTINE_CO_SCHEDULER_WORKER_TYPE_H_