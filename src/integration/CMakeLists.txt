set(PROGRAM_NAME pico_w_web_webserver)
set(PICO_BOARD pico_w)

pico_sdk_init()

message("Running makefsdata python script")
execute_process(COMMAND
    py makefsdata.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(integration
        main.c
        )

target_compile_definitions(integration PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )

target_include_directories(integration PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
        )

target_link_libraries(integration 
        pico_stdlib 
        hardware_adc 
        hardware_pwm
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_iperf
        FreeRTOS-Kernel-Heap4
        hardware_i2c
        pico_lwip_http
        )


# create map/bin/hex file etc.
pico_add_extra_outputs(integration)

pico_enable_stdio_usb(integration 1)
pico_enable_stdio_uart(integration 0)
