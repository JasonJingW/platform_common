#include "system_init.h"
#include "stm32f7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#if configTICK_RATE_HZ != 1000U
#error "Shared HAL/FreeRTOS SysTick requires configTICK_RATE_HZ == 1000"
#endif

void xPortSysTickHandler(void);

void hardware_init(void)
{
    /* Configure only the caches provided by the selected device. */
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_DisableDCache();
#endif
#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    SCB_EnableICache();
#endif
    HAL_Init();
}

/* STM32F7 startup vectors dispatch to these chip-specific handlers. */
void SysTick_Handler(void)
{
    HAL_IncTick();
    /* HAL uses SysTick before the scheduler starts; RTOS state is not ready yet. */
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
}

void HardFault_Handler(void)
{
    __disable_irq();
    while (1) { }
}
