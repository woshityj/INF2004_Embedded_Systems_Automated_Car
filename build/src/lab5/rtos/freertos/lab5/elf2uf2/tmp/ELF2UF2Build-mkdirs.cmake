# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/KuangYi/Desktop/EmbeddedSystems/pico/pico-sdk/tools/elf2uf2"
  "/Users/KuangYi/Desktop/EmbeddedSystems/INF2004_Embedded_Systems/build/elf2uf2"
  "/Users/KuangYi/Desktop/EmbeddedSystems/INF2004_Embedded_Systems/build/src/lab5/rtos/freertos/lab5/elf2uf2"
  "/Users/KuangYi/Desktop/EmbeddedSystems/INF2004_Embedded_Systems/build/src/lab5/rtos/freertos/lab5/elf2uf2/tmp"
  "/Users/KuangYi/Desktop/EmbeddedSystems/INF2004_Embedded_Systems/build/src/lab5/rtos/freertos/lab5/elf2uf2/src/ELF2UF2Build-stamp"
  "/Users/KuangYi/Desktop/EmbeddedSystems/INF2004_Embedded_Systems/build/src/lab5/rtos/freertos/lab5/elf2uf2/src"
  "/Users/KuangYi/Desktop/EmbeddedSystems/INF2004_Embedded_Systems/build/src/lab5/rtos/freertos/lab5/elf2uf2/src/ELF2UF2Build-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/KuangYi/Desktop/EmbeddedSystems/INF2004_Embedded_Systems/build/src/lab5/rtos/freertos/lab5/elf2uf2/src/ELF2UF2Build-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/KuangYi/Desktop/EmbeddedSystems/INF2004_Embedded_Systems/build/src/lab5/rtos/freertos/lab5/elf2uf2/src/ELF2UF2Build-stamp${cfgdir}") # cfgdir has leading slash
endif()
