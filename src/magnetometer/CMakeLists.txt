cmake_minimum_required(VERSION 3.13)

# include(pico_sdk_import.cmake)


# project(lsm303dlh_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX-STANDARD 17)
pico_sdk_init()

add_executable(magnetometer
   magnetometer.c
   lsm303dlh
   lsm303dlh.c
)

# enable usb output, disable uart output
pico_enable_stdio_usb(magnetometer 1)
pico_enable_stdio_uart(magnetometer 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(magnetometer)

# pull in common dependencies and additional i2c hardware support
target_link_libraries(magnetometer pico_stdlib hardware_i2c)