## Motor Driver Module
### Introduction
This sub-module library contains:
1. Magnetometer control logic `magnetometer_driver.c`
# Magnetometer Driver

## Objective

The objective of this project is to provide a clear and concise interface for interfacing with the GY-511 magnetometer sensor using the Raspberry Pi Pico. This driver simplifies the process of setting up and reading data from the magnetometer, making it easier to integrate magnetometer data into your projects.

## Explanation

The Magnetometer Driver is designed to work with the GY-511 magnetometer sensor, providing an easy-to-use interface for configuring the sensor and retrieving magnetic field data. Below is an explanation of the key components and functions of this driver:

### Header File (magnetometer_driver.h)
- **Definitions**: It defines several constants for I2C communication and register addresses used by the magnetometer.
- **Data Structure**: The `mag_t` structure is defined to store the X, Y, and Z-axis magnetometer data.
- **Function Prototypes**: Function prototypes for the driver functions are declared in this header file. These functions include:
  - `void create_mag_setup()`: Initializes and configures the magnetometer.
  - `void read_mag(mag_t *mag)`: Reads magnetometer data and stores it in the `mag_t` structure.
  - `int32_t get_angle(mag_t *mag)`: Calculates the angle based on magnetometer data.

### Implementation Files (.c)
- **Magnetometer Setup**: The `create_mag_setup()` function initialises and configures the magnetometer with the desired settings, such as refresh rate and gain.
- **Reading Magnetometer Data**: The `read_mag(mag_t *mag)` function reads magnetometer data and stores it in the provided `mag_t` structure.
- **Calculating Angle**: The `get_angle(mag_t *mag)` function calculates the angle from the magnetometer data in degrees.
- **I2C Configuration**: The code sets up and configures the I2C communication with the specified baud rate.
- **Orientation Determination**: The main application reads magnetometer data, calculates the angle, normalizes it to 0-360 degrees, and determines the orientation (North, East, South, or West).
- **Sleep Function**: A sleep function is used to introduce a delay between readings.
- **Error Handling**: Error handling for I2C communication is not included in this code and should be added as needed.
### Flowchart
-- To be included --
### Header Files
Definitions in `magnetometer_driver.h`:

```c
#define INTERFACE_B 0x1E // Slave address for the magnetometer

#define CTRL_REG_1 0x20 // Control register 1 address

#define CTRL_REG_4 0x23 // Control register 4 address

#define MAG_REG 0x03    // Magnetometer data register address with auto-increment

#define MAG_CRA 0x00    // Configuration register A address for magnetometer

#define MAG_CRB 0x01    // Configuration register B address for magnetometer

#define MAG_MR 0x02     // Mode register address for magnetometer

typedef struct {

   int16_t x; // X-axis magnetometer data

   int16_t y; // Y-axis magnetometer data

   int16_t z; // Z-axis magnetometer data

} mag_t;

void create_mag_setup();     // Function prototype for magnetometer setup

void read_mag(mag_t *mag);   // Function prototype to read magnetometer data

int32_t get_angle(mag_t *mag);  // Function prototype to calculate an angle from magnetometer data

```