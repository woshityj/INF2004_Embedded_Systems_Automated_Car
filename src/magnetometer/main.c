#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "magnetometer_driver.h"

#define I2C_BAUD 400 // I2C baud rate (400 kHz)
#define REFRESH_PERIOD 100 // Refresh period in milliseconds

// Function declaration
void init_i2c_default();
void get_readings();

// main ------------------------------------------------------------------
int main() {
   // Check for default GPIO setup
   #if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
      #warning Default I2C pins were not defined
      puts("Default I2C pins were not defined");
   #else
   // Default I2C, output setup
   init_i2c_default();
   stdio_init_all();

   // Setup device
   create_mag_setup();
   get_readings();
   #endif
   return 0;
}

// function --------------------------------------------------------------
void init_i2c_default() {
   // Initialize I2C with the default settings
   i2c_init(i2c_default, I2C_BAUD * 1000);
   gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
   gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
   gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
   gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}

void get_readings() {
    mag_t mag;
    // Get initial angle
   read_mag(&mag);
   int32_t start_angle = get_angle(&mag);
    while (true) {
       read_mag(&mag);
       int32_t angle = get_angle(&mag);
       int32_t used_angle = angle - start_angle;

       // Normalize the angle to 0-360
       if (used_angle < 0) {
           used_angle += 360;
       }

       // Determine orientation
       char* orientation;
       if (used_angle >= 315 || used_angle < 45) {
           orientation = "North";
       } else if (used_angle >= 45 && used_angle < 135) {
           orientation = "East";
       } else if (used_angle >= 135 && used_angle < 225) {
           orientation = "South";
       } else if (used_angle >= 225 && used_angle < 315) {
           orientation = "West";
       }

       printf("Mag. X =%4d Y =%4d, Z =%4d, Orientation: %s\n",
              mag.x, mag.y, mag.z, orientation);
    //    printf("Relative Degree: %4d, True Degree: %4d, Orientation: %s\n",
    //           used_angle, angle, orientation);
       sleep_ms(REFRESH_PERIOD);
   }
}
/* end of main.c */