#ifndef __LSM303DLH__ // Header guard to prevent multiple inclusions
#define __LSM303DLH__

#include <stdint.h>

#define INTERFACE_B 0x1E // Slave address for the magnetometer

#define CTRL_REG_1 0x20 // Control register 1 address
#define CTRL_REG_4 0x23 // Control register 4 address
#define MAG_REG 0x03    // Magnetometer data register address with auto-increment
#define MAG_CRA 0x00    // Configuration register A address for magnetometer
#define MAG_CRB 0x01    // Configuration register B address for magnetometer
#define MAG_MR 0x02     // Mode register address for magnetometer

typedef struct {
   int16_t x; // X-axis magnetometer data
   int16_t y; // Y-axis magnetometer data
   int16_t z; // Z-axis magnetometer data
} mag_t;

void create_mag_setup();     // Function prototype for magnetometer setup
void read_mag(mag_t *mag);   // Function prototype to read magnetometer data
int32_t get_angle(mag_t *mag);  // Function prototype to calculate an angle from magnetometer data

#endif
/* end of magnetometer_driver.h */
