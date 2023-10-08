if (EXISTS ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c)
    add_executable(lab5_nosys
            2201354.c
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c
            )
    target_compile_definitions(lab5_nosys PRIVATE
            WIFI_SSID=\"${WIFI_SSID}\"
            WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
            )
    target_include_directories(lab5_nosys PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping
            )
    target_link_libraries(lab5_nosys
            pico_cyw43_arch_lwip_threadsafe_background
            pico_stdlib
            pico_lwip_iperf
            FreeRTOS-Kernel-Heap4 
            hardware_adc
            # FreeRTOS kernel and dynamic heap
            )
    pico_add_extra_outputs(lab5_nosys)

    add_executable(lab5_sys
            2201354.c
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c
            )
    target_compile_definitions(lab5_sys PRIVATE
            WIFI_SSID=\"${WIFI_SSID}\"
            WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
            NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
            LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
            PING_USE_SOCKETS=1
            )
    target_include_directories(lab5_sys PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping
            )
    target_link_libraries(lab5_sys 
        pico_cyw43_arch_lwip_sys_freertos 
        pico_stdlib pico_lwip_iperf 
        FreeRTOS-Kernel-Heap4 
        hardware_adc) # FreeRTOS kernel and dynamic heap

    pico_enable_stdio_usb(lab5_sys 1)
    pico_add_extra_outputs(lab5_sys)
endif()