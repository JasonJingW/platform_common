#include "system_init.h"
#include "stm32f7xx_hal.h"

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
