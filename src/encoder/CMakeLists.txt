add_executable(encoder_driver
        main.c
        )

target_link_libraries(encoder_driver pico_stdlib)

# pull in common dependencies and additional pwm hardware support
target_link_libraries(encoder_driver pico_stdlib hardware_pwm)

pico_enable_stdio_usb(encoder_driver 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(encoder_driver)
