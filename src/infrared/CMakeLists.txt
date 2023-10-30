add_executable(infrared infrared.c)
add_executable(readadc readadc.c)
add_executable(main main.c)

# pull in common dependencies
target_link_libraries(infrared pico_stdlib hardware_adc)
target_link_libraries(readadc pico_stdlib hardware_adc)
target_link_libraries(main pico_stdlib hardware_adc)

# enable usb output, enable uart output
pico_enable_stdio_usb(infrared 1)
pico_enable_stdio_usb(readadc 1)
pico_enable_stdio_usb(main 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(infrared)
pico_add_extra_outputs(readadc)
pico_add_extra_outputs(main)