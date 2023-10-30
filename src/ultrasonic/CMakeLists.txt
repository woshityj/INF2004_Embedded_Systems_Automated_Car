add_executable(ultrasonic
        main.c
        )

target_link_libraries(ultrasonic pico_stdlib hardware_pwm)

# purely just for making the library
add_library(ultrasoniclib ultrasonic.c)
target_link_libraries(ultrasoniclib pico_stdlib hardware_pwm)
# pull in common dependencies and additional pwm hardware support

# create map/bin/hex file etc.
pico_add_extra_outputs(ultrasonic)

pico_enable_stdio_usb(ultrasonic 1)