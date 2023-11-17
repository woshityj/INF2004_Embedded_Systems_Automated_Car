add_executable(motor_and_encoder
    motor_and_encoder.c
        )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(motor_and_encoder pico_stdlib hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(motor_and_encoder)

pico_enable_stdio_usb(motor_and_encoder 1)