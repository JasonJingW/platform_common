#include "system_init.h"
#include "stm32f7xx_hal.h"
#include "stm32f767xgx_config.h"
#include "FreeRTOS.h"
#include "task.h"

void xPortSysTickHandler(void);

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

static void system_clck_init(void)
{
    /* 25 MHz HSE -> 216 MHz SYSCLK. */
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState = RCC_HSE_ON;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLM = 25;
    osc.PLL.PLLN = 432;
    osc.PLL.PLLP = RCC_PLLP_DIV2;
    osc.PLL.PLLQ = 9;
    clk.ClockType =
        RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV4;
    clk.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK || HAL_PWREx_EnableOverDrive() != HAL_OK ||
        HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_7) != HAL_OK)
    {
        /* Stop startup so a debugger can inspect the clock and power registers. */
        for (;;)
        {
        }
    }
}

void hardware_init(void)
{
    /* Write back any inherited cache contents before changing MPU attributes. */
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U)
    {
        SCB_DisableDCache();
    }

    init_noncacheable_ram();
    SCB_EnableDCache();
    SCB_EnableICache();
    HAL_Init();
    system_clck_init();
}

void SysTick_Handler(void)
{
    HAL_IncTick();
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}
