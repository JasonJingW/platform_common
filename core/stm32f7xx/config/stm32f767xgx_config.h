#ifndef STM32F767XGX_CONFIG_H
#define STM32F767XGX_CONFIG_H

/* Physical capacities for STM32F767xGx, not linker memory placement. */
#define CHIP_FLASH_SIZE_BYTES             (1024U * 1024U)
#define CHIP_SRAM1_SIZE_BYTES             (368U * 1024U)
#define CHIP_SRAM2_SIZE_BYTES             (16U * 1024U)
#define CHIP_DTCM_SIZE_BYTES              (128U * 1024U)
#define CHIP_SYSTEM_RAM_SIZE_BYTES        \
    (CHIP_SRAM1_SIZE_BYTES + CHIP_SRAM2_SIZE_BYTES + CHIP_DTCM_SIZE_BYTES)
#define CHIP_ITCM_SIZE_BYTES              (16U * 1024U)
#define CHIP_BACKUP_SRAM_SIZE_BYTES       (4U * 1024U)

/* Adjustable RTOS budgets; stack depths are 32-bit words, heap is bytes. */
#ifndef CHIP_RTOS_TOTAL_HEAP_SIZE
#define CHIP_RTOS_TOTAL_HEAP_SIZE         (32U * 1024U)
#endif
#ifndef CHIP_RTOS_MINIMAL_STACK_SIZE
#define CHIP_RTOS_MINIMAL_STACK_SIZE      128U
#endif
#ifndef CHIP_RTOS_TIMER_TASK_STACK_DEPTH
#define CHIP_RTOS_TIMER_TASK_STACK_DEPTH  256U
#endif
#ifndef CHIP_RTOS_TIMER_QUEUE_LENGTH
#define CHIP_RTOS_TIMER_QUEUE_LENGTH     10U
#endif
#ifndef CHIP_RTOS_MAX_PRIORITIES
#define CHIP_RTOS_MAX_PRIORITIES          8U
#endif
#ifndef CHIP_RTOS_MAX_TASK_NAME_LEN
#define CHIP_RTOS_MAX_TASK_NAME_LEN       16U
#endif
#ifndef CHIP_RTOS_TIMER_TASK_PRIORITY
#define CHIP_RTOS_TIMER_TASK_PRIORITY     2U
#endif
#ifndef CHIP_RTOS_APP_TASK_STACK_DEPTH
#define CHIP_RTOS_APP_TASK_STACK_DEPTH    512U
#endif

#endif
