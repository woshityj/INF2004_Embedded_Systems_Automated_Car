add_executable(magnetometer_driver
   main.c
   magnetometer_driver.c
)

# enable usb output, disable uart output
pico_enable_stdio_usb(magnetometer_driver 1)
pico_enable_stdio_uart(magnetometer_driver 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(magnetometer_driver)

# pull in common dependencies and additional i2c hardware support
target_link_libraries(magnetometer_driver pico_stdlib hardware_i2c)
