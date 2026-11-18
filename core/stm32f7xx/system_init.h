#ifndef PLATFORM_STM32F7XX_SYSTEM_INIT_H
#define PLATFORM_STM32F7XX_SYSTEM_INIT_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* Initialize MPU, caches, HAL and system clocks. */
    void hardware_init(void);

#ifdef __cplusplus
}
#endif

#endif
