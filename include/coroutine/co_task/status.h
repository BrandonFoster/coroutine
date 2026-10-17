/**
 * @file coroutine/co_task/status.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the co_task_status enumeration for co_task coroutine statuses.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common enumeration used to represent stackless coroutine states.
 */

#ifndef COROUTINE_CO_TASK_STATUS_H_
#define COROUTINE_CO_TASK_STATUS_H_

typedef enum {
  CO_TASK_PENDING,
  CO_TASK_YIELDED,
  CO_TASK_COMPLETED
} co_task_status;

#endif // COROUTINE_CO_TASK_STATUS_H_