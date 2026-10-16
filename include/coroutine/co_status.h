/**
 * @file coroutine/co_status.h
 * @author Brandon Foster (https://github.com/BrandonFoster)
 * @brief Declares the co_status enumeration for coroutine states.
 *
 * Copyright (c) 2026 Brandon Foster.
 *
 * Declares the common enumeration used to represent coroutine states.
 */

#ifndef CO_STATUS_H_
#define CO_STATUS_H_

typedef enum {
  CO_READY,
  CO_RUNNING,
  CO_SUSPENDED,
  CO_DEAD
} co_status;

#endif // CO_STATUS_H_