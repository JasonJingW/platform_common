#include "system_init.h"
#include "stm32f7xx_hal.h"
#include "stm32f767xgx_config.h"
#if PROJECT_USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"

#if configTICK_RATE_HZ != 1000U
#error "Shared HAL/FreeRTOS SysTick requires configTICK_RATE_HZ == 1000"
#endif

void xPortSysTickHandler(void);
#endif

static void init_noncacheable_ram(void)
{
    MPU_Region_InitTypeDef region = {
        .Enable = MPU_REGION_ENABLE,
        .Number = MPU_REGION_NUMBER0,
        .BaseAddress = (uint32_t)CHIP_NONCACHE_RAM_BASE,
        .Size = MPU_REGION_SIZE_16KB,
        .SubRegionDisable = 0U,
        .TypeExtField = MPU_TEX_LEVEL1,
        .AccessPermission = MPU_REGION_FULL_ACCESS,
        .DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE,
        .IsShareable = MPU_ACCESS_SHAREABLE,
        .IsCacheable = MPU_ACCESS_NOT_CACHEABLE,
        .IsBufferable = MPU_ACCESS_NOT_BUFFERABLE,
    };

    HAL_MPU_Disable();
    HAL_MPU_ConfigRegion(&region);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void hardware_init(void)
{
    /* Write back any inherited cache contents before changing MPU attributes. */
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U) {
        SCB_DisableDCache();
    }
    init_noncacheable_ram();
    SCB_EnableDCache();
    SCB_EnableICache();
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
