/**
 * @file coroutine/co_thread/status.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the co_thread_status enumeration for co_thread coroutine statuses.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common enumeration used to represent stackful coroutine states.
 */

#ifndef COROUTINE_CO_THREAD_STATUS_H_
#define COROUTINE_CO_THREAD_STATUS_H_

typedef enum {
  CO_THREAD_READY,
  CO_THREAD_RUNNING,
  CO_THREAD_SUSPENDED,
  CO_THREAD_FINISHED
} co_thread_status;

#endif // COROUTINE_CO_THREAD_STATUS_H_