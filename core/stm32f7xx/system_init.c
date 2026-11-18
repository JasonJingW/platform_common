#include "system_init.h"
#include "stm32f7xx_hal.h"
#if PROJECT_USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"

#if configTICK_RATE_HZ != 1000U
#error "Shared HAL/FreeRTOS SysTick requires configTICK_RATE_HZ == 1000"
#endif

void xPortSysTickHandler(void);
#endif

void hardware_init(void)
{
    /* Configure only the caches provided by the selected device. */
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    /* Keep RTT RAM coherent without cleaning uninitialized cache lines at boot. */
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U) {
        SCB_DisableDCache();
    }
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
#if PROJECT_USE_FREERTOS
    /* HAL uses SysTick before the scheduler starts; RTOS state is not ready yet. */
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
#endif
}

void HardFault_Handler(void)
{
    __disable_irq();
    while (1) { }
}
