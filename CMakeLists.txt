# Source interfaces only. The parent build chooses which interfaces to consume.
set(_common_chip "${CMAKE_CURRENT_LIST_DIR}/core/stm32f7xx")
set(_common_hal "${_common_chip}/hal_driver")
set(_common_cmsis "${_common_chip}/cmsis")
add_library(common_stm32f7xx INTERFACE)
add_library(common::stm32f7xx ALIAS common_stm32f7xx)
target_include_directories(common_stm32f7xx INTERFACE
    "${_common_chip}" "${_common_chip}/config" "${_common_hal}/Inc"
    "${_common_cmsis}/Core/Include" "${_common_cmsis}/Device/ST/STM32F7xx/Include")
target_sources(common_stm32f7xx INTERFACE
    "${_common_chip}/system_init.c"
    "${_common_cmsis}/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c"
    "${_common_cmsis}/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f767xx.s")
foreach(_source IN ITEMS stm32f7xx_hal stm32f7xx_hal_cortex stm32f7xx_hal_flash
        stm32f7xx_hal_flash_ex stm32f7xx_hal_gpio stm32f7xx_hal_pwr
        stm32f7xx_hal_pwr_ex stm32f7xx_hal_rcc stm32f7xx_hal_rcc_ex)
    target_sources(common_stm32f7xx INTERFACE "${_common_hal}/Src/${_source}.c")
endforeach()

set(_common_rtos "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS")
add_library(common_freertos_core INTERFACE)
add_library(common::freertos_core ALIAS common_freertos_core)
target_include_directories(common_freertos_core INTERFACE "${_common_rtos}/include")
foreach(_source IN ITEMS tasks queue list event_groups stream_buffer)
    target_sources(common_freertos_core INTERFACE "${_common_rtos}/${_source}.c")
endforeach()
foreach(_component IN ITEMS timers heap4 port_cm7 port_cm4f)
    add_library(common_freertos_${_component} INTERFACE)
    add_library(common::freertos_${_component} ALIAS common_freertos_${_component})
endforeach()
target_sources(common_freertos_timers INTERFACE "${_common_rtos}/timers.c")
target_sources(common_freertos_heap4 INTERFACE "${_common_rtos}/portable/MemMang/heap_4.c")
target_sources(common_freertos_port_cm7 INTERFACE "${_common_rtos}/portable/GCC/ARM_CM7/r0p1/port.c")
target_include_directories(common_freertos_port_cm7 INTERFACE "${_common_rtos}/portable/GCC/ARM_CM7/r0p1")
target_sources(common_freertos_port_cm4f INTERFACE "${_common_rtos}/portable/GCC/ARM_CM4F/port.c")
target_include_directories(common_freertos_port_cm4f INTERFACE "${_common_rtos}/portable/GCC/ARM_CM4F")

add_library(common_rtt INTERFACE)
add_library(common::rtt ALIAS common_rtt)
target_sources(common_rtt INTERFACE "${CMAKE_CURRENT_LIST_DIR}/RTT/RTT/SEGGER_RTT.c"
    "${CMAKE_CURRENT_LIST_DIR}/RTT/RTT/SEGGER_RTT_printf.c")
target_include_directories(common_rtt INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/RTT/RTT" "${CMAKE_CURRENT_LIST_DIR}/RTT/Config")
