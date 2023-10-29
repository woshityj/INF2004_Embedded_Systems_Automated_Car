add_executable(motor_driver
        main.c
        )

add_library(pid pid.c)
add_library(motor_driver motor_driver.c)

target_link_libraries(pid pico_stdlib)

# pull in common dependencies and additional pwm hardware support
target_link_libraries(motor pico_stdlib hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(motor)

pico_enable_stdio_usb(motor 1)