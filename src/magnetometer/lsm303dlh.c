#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lsm303dlh.h"

// Function to set up the accelerometer
void lsm303dlh_acc_setup() {
   uint8_t buffer[2] = {CTRL_REG_4, 0x00};
   
   // Write configuration to the accelerometer
   i2c_write_blocking(
      i2c_default,
      INTERFACE_A,
      buffer,
      2,
      true
   );

   buffer[0] = CTRL_REG_1;
   buffer[1] = 0x27;
   
   i2c_write_blocking(
      i2c_default,
      INTERFACE_A,
      buffer,
      2,
      false
   );
}

// Function to set up the magnetometer
void lsm303dlh_mag_setup() {
   uint8_t buffer[2] = {MAG_CRA, 0x10}; // 15 Hz refresh rate
   
   // Write configuration to the magnetometer
   i2c_write_blocking(
      i2c_default,
      INTERFACE_B,
      buffer,
      2,
      true
   );

   buffer[0] = MAG_CRB;
   buffer[1] = 0xE0; // Gain - range +-8.1 Gauss, Gain X/Y and Z [LSB/Gauss] 230, GainZ [LSB/Gauss] 205
   
   i2c_write_blocking(
      i2c_default,
      INTERFACE_B,
      buffer,
      2,
      true
   );

   buffer[0] = MAG_MR;
   buffer[1] = 0x00; // Continuous-conversion mode
   
   i2c_write_blocking(
      i2c_default,
      INTERFACE_B,
      buffer,
      2,
      false
   );
}

// Function to read accelerometer data
void lsm303dlh_read_acc(accel_t *acc) {
   uint8_t buffer[6];
   int16_t accel[3];
   uint8_t reg = ACC_REG;

   // Write the accelerometer register address
   i2c_write_blocking(
      i2c_default,
      INTERFACE_A,
      &reg,
      1,
      true
   );

   // Read accelerometer data
   i2c_read_blocking(
      i2c_default,
      INTERFACE_A,
      buffer,
      6,
      false
   );

   // Merge uint8_t values into int16_t
   for (int i = 0; i < 3; i++) {
      accel[i] = ((buffer[i * 2] | buffer[(i * 2) + 1]  << 8));
   }

   acc->x = accel[0];
   acc->y = accel[1];
   acc->z = accel[2];
}

// Function to read magnetometer data
void lsm303dlh_read_mag(mag_t *mag) {
   uint8_t buffer[6];
   int16_t magnet[3];
   uint8_t reg = MAG_REG;

   // Write the magnetometer register address
   i2c_write_blocking(
      i2c_default,
      INTERFACE_B,
      &reg,
      1,
      true
   );

   // Read magnetometer data
   i2c_read_blocking(
      i2c_default,
      INTERFACE_B,
      buffer,
      6,
      false
   );

   // Merge uint8_t values into int16_t
   for (int i = 0; i < 3; i++) {
      magnet[i] = ((buffer[i * 2] << 8 | buffer[(i * 2) + 1] ));
   }

   // Save x, y, z values into mag struct
   mag->x = magnet[0];
   mag->y = magnet[1];
   mag->z = magnet[2];
}

// Function to calculate the angle from magnetometer data
int32_t get_angle(mag_t *mag) {
   // Calculate angle in radians
   double angle_rad = atan2((double)mag->y, (double)mag->x);

   // Convert angle to degrees
   int32_t angle_deg = (int32_t)(angle_rad * (180.0 / M_PI));

   // Normalize the angle to 0-360
   if (angle_deg < 0) {
       angle_deg += 360;
   }

   return angle_deg;
}

/* end of lsm303dlh.c */
