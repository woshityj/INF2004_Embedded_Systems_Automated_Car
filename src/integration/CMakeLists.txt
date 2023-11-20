add_executable(integration
        main.c
        )

target_compile_definitions(integration PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSOWRD=\"${WIFI_PASSWORD}\"
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
        )


# create map/bin/hex file etc.
pico_add_extra_outputs(integration)

pico_enable_stdio_usb(integration 1)
