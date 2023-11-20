add_subdirectory(pathfinder)
if (EXISTS ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c)
        add_executable(autonomousCar_nosys
                main.c
                ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c
                )
        target_compile_definitions(autonomousCar_nosys PRIVATE
                WIFI_SSID=\"${WIFI_SSID}\"
                WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
                )
        target_include_directories(autonomousCar_nosys PRIVATE
                ${CMAKE_CURRENT_LIST_DIR}
                ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
                ${PICO_LWIP_CONTRIB_PATH}/apps/ping
                )
        target_link_libraries(autonomousCar_nosys
                pico_cyw43_arch_lwip_threadsafe_background
                pico_stdlib
                pico_lwip_iperf
                FreeRTOS-Kernel-Heap4 
                hardware_adc
                hardware_pwm
                pathfinder
                # FreeRTOS kernel and dynamic heap
                )
        pico_add_extra_outputs(autonomousCar_nosys)

        pico_enable_stdio_usb(autonomousCar_nosys 1)
        
    add_executable(autonomousCar_sys
            main.c
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c
            )
    target_compile_definitions(autonomousCar_sys PRIVATE
            WIFI_SSID=\"${WIFI_SSID}\"
            WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
            NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
            LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
            PING_USE_SOCKETS=1
            )
    target_include_directories(autonomousCar_sys PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/lwips/ # for our common lwipopts
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping
            )
    target_link_libraries(autonomousCar_sys 
        pico_cyw43_arch_lwip_sys_freertos 
        pico_stdlib pico_lwip_iperf 
        FreeRTOS-Kernel-Heap4 
        hardware_adc
        hardware_pwm
        pathfinder
        ) # FreeRTOS kernel and dynamic heap

    pico_enable_stdio_usb(autonomousCar_sys 1)
    pico_add_extra_outputs(autonomousCar_sys)
endif()

if (NOT FREERTOS_KERNEL_PATH AND NOT DEFINED ENV{FREERTOS_KERNEL_PATH})
    message("Skipping Pico W FreeRTOS examples as FREERTOS_KERNEL_PATH not defined")
else()
    include(FreeRTOS_Kernel_import.cmake)
    add_subdirectory(integration)
endif()

add_subdirectory(infrared)
add_subdirectory(encoder)
add_subdirectory(motor)
add_subdirectory(ultrasonic)
add_subdirectory(magnetometer)
add_subdirectory(wifi)
add_subdirectory(fusion)
add_subdirectory(webserver_and_wifi)
