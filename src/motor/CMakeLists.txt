add_executable(motor_driver_test
        main.c
        )

add_executable(motor_driver
        motor_driver.c
        )

add_library(pid pid.c)

target_link_libraries(pid pico_stdlib)

# pull in common dependencies and additional pwm hardware support
target_link_libraries(motor_driver_test pico_stdlib hardware_pwm)

target_link_libraries(motor_driver pico_stdlib hardware_pwm)


# create map/bin/hex file etc.
pico_add_extra_outputs(motor_driver)

pico_enable_stdio_usb(motor_driver 1)