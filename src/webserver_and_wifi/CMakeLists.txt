
set(PROGRAM_NAME pico_w_web_webserver)
set(PICO_BOARD pico_w)

pico_sdk_init()

message("Running makefsdata python script")
execute_process(COMMAND
    py makefsdata.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(webserver
    webserver.c
)

target_include_directories(webserver PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(webserver pico_cyw43_arch_lwip_threadsafe_background pico_lwip_http pico_stdlib hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(webserver)

pico_enable_stdio_usb(webserver 1)
pico_enable_stdio_uart(webserver 0)