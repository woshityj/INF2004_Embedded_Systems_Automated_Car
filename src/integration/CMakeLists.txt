add_executable(integration
        main.c
        )

target_link_libraries(integration pico_stdlib hardware_adc hardware_pwm)


# create map/bin/hex file etc.
pico_add_extra_outputs(integration)

pico_enable_stdio_usb(integration 1)
