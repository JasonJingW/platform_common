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

foreach(_driver IN ITEMS dma eth i2c sai uart)
    add_library(common_stm32f7xx_${_driver} INTERFACE)
    add_library(common::stm32f7xx_${_driver} ALIAS common_stm32f7xx_${_driver})
    target_sources(common_stm32f7xx_${_driver} INTERFACE "${_common_hal}/Src/stm32f7xx_hal_${_driver}.c")
    string(TOUPPER "${_driver}" _module)
    target_compile_definitions(common_stm32f7xx_${_driver} INTERFACE HAL_${_module}_MODULE_ENABLED)
endforeach()
target_sources(common_stm32f7xx_i2c INTERFACE "${_common_hal}/Src/stm32f7xx_hal_i2c_ex.c")
target_sources(common_stm32f7xx_uart INTERFACE "${_common_hal}/Src/stm32f7xx_hal_uart_ex.c")
target_link_libraries(common_stm32f7xx_i2c INTERFACE common::stm32f7xx_dma)
target_link_libraries(common_stm32f7xx_sai INTERFACE common::stm32f7xx_dma)
target_link_libraries(common_stm32f7xx_uart INTERFACE common::stm32f7xx_dma)

set(_common_lwip "${CMAKE_CURRENT_LIST_DIR}/lwip/src")
add_library(common_lwip INTERFACE)
add_library(common::lwip ALIAS common_lwip)
target_include_directories(common_lwip INTERFACE "${_common_lwip}/include")
foreach(_source IN ITEMS core/def core/inet_chksum core/init core/ip core/mem core/memp
        core/netif core/pbuf core/stats core/sys core/timeouts core/udp core/tcp core/tcp_in core/tcp_out
        core/ipv4/etharp core/ipv4/icmp core/ipv4/ip4 core/ipv4/ip4_addr netif/ethernet)
    target_sources(common_lwip INTERFACE "${_common_lwip}/${_source}.c")
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
