cmake_minimum_required(VERSION 3.30.3)

set(PICO_BOARD pico_w)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pwm_motor_control C CXX ASM)
set(C_STANDARD_REQUIRED 11)
set(CXX_STANDARD_REQUIRED 17)

pico_sdk_init()

file(GLOB_RECURSE SRC_FILES CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_cyw43_arch_none
    hardware_pwm
    hardware_gpio
    hardware_adc
    hardware_watchdog
    hardware_flash
    hardware_sync
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})