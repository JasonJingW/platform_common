#ifndef STM32F767XGX_CONFIG_H
#define STM32F767XGX_CONFIG_H

#include <stdint.h>

/* Physical capacities for STM32F767xGx, not linker memory placement. */
/* The GCC build also reads these capacities in (KiB * 1024U) form. */
#define CHIP_FLASH_SIZE_BYTES (1024U * 1024U)
#define CHIP_SRAM1_SIZE_BYTES (368U * 1024U)
#define CHIP_SRAM2_SIZE_BYTES (16U * 1024U)
#define CHIP_DTCM_SIZE_BYTES (128U * 1024U)
#define CHIP_SYSTEM_RAM_SIZE_BYTES                                                                 \
    (CHIP_SRAM1_SIZE_BYTES + CHIP_SRAM2_SIZE_BYTES + CHIP_DTCM_SIZE_BYTES)
#define CHIP_ITCM_SIZE_BYTES (16U * 1024U)
#define CHIP_BACKUP_SRAM_SIZE_BYTES (4U * 1024U)

/* Shared RAM bounds come from the linker; END is exclusive. */
extern uint8_t __noncache_start__[];
extern uint8_t __noncache_end__[];
#define CHIP_NONCACHE_RAM_BASE ((uintptr_t)__noncache_start__)
#define CHIP_NONCACHE_RAM_END ((uintptr_t)__noncache_end__)
#define CHIP_NONCACHE_RAM_SIZE_BYTES (16U * 1024U)

/* Place a static-storage DMA object in the non-cacheable shared RAM. */
/* The NOLOAD section is not initialized at boot. Apply peripheral alignment separately. */
#define __DMA_DATA __attribute__((section(".noncache.dma")))
/* Define a fixed-size DMA array. Count is in elements. */
#define __DMA_U8_DEFINE(name, count) static uint8_t name[(count)] __DMA_DATA
#define __DMA_U16_DEFINE(name, count) static uint16_t name[(count)] __DMA_DATA
#define __DMA_U32_DEFINE(name, count) static uint32_t name[(count)] __DMA_DATA

/* Adjustable budgets: stack depths are words, heap size is bytes. */
#define CHIP_RTOS_TOTAL_HEAP_SIZE (64U * 1024U)
#define CHIP_RTOS_MINIMAL_STACK_SIZE (128U)
#define CHIP_RTOS_TIMER_TASK_STACK_DEPTH (256U)
#define CHIP_RTOS_TIMER_QUEUE_LENGTH (10U)
#define CHIP_RTOS_MAX_PRIORITIES (8U)
#define CHIP_RTOS_MAX_TASK_NAME_LEN (16U)
#define CHIP_RTOS_TIMER_TASK_PRIORITY (2U)

#endif
