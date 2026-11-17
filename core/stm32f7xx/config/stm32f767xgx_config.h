#ifndef STM32F767XGX_CONFIG_H
#define STM32F767XGX_CONFIG_H

/* Physical capacities for STM32F767xGx, not linker memory placement. */
/* Keep capacities as decimal byte literals: the GCC linker reads these too. */
#define CHIP_FLASH_SIZE_BYTES             (1048576U)
#define CHIP_SRAM1_SIZE_BYTES             (376832U)
#define CHIP_SRAM2_SIZE_BYTES             (16384U)
#define CHIP_DTCM_SIZE_BYTES              (131072U)
#define CHIP_SYSTEM_RAM_SIZE_BYTES        \
    (CHIP_SRAM1_SIZE_BYTES + CHIP_SRAM2_SIZE_BYTES + CHIP_DTCM_SIZE_BYTES)
#define CHIP_ITCM_SIZE_BYTES              (16384U)
#define CHIP_BACKUP_SRAM_SIZE_BYTES       (4096U)

/* Adjustable budgets: stack depths are words, heap size is bytes. */
#define CHIP_RTOS_TOTAL_HEAP_SIZE         (32U * 1024U)
#define CHIP_RTOS_MINIMAL_STACK_SIZE      (128U)
#define CHIP_RTOS_TIMER_TASK_STACK_DEPTH  (256U)
#define CHIP_RTOS_TIMER_QUEUE_LENGTH     (10U)
#define CHIP_RTOS_MAX_PRIORITIES          (8U)
#define CHIP_RTOS_MAX_TASK_NAME_LEN       (16U)
#define CHIP_RTOS_TIMER_TASK_PRIORITY     (2U)

#endif
