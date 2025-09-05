# Add executable. Default name is the project name, version 0.1

add_executable(focpwm focpwm.cpp )

pico_set_program_name(focpwm "focpwm")
pico_set_program_version(focpwm "0.1")

# Generate PIO header
pico_generate_pio_header(focpwm ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(focpwm 0)
pico_enable_stdio_usb(focpwm 1)

# Add the standard library to the build
target_link_libraries(focpwm
        pico_async_context_freertos
        FreeRTOS-Kernel-Heap4
        pico_stdlib
)

# Add the standard include files to the build
target_include_directories(focpwm PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../lib/FreeRTOS
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(focpwm 
        hardware_pio
        hardware_timer
        hardware_clocks
        )

pico_add_extra_outputs(focpwm)

