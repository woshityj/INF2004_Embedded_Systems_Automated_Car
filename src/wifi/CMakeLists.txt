add_executable(wifi
        wifi_driver.c
        )

target_include_directories(wifi PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../lwips/ # for our common lwipopts
        )
target_link_libraries(wifi
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        )

pico_add_extra_outputs(wifi)
pico_enable_stdio_usb(wifi 1)
