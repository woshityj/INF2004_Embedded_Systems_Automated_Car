## Encoder Driver Module

### Introduction

This sub-module library contains:

1. Encoder Logic `encoder_driver.c`

### Objectives
The objective of the WYC-H206 IR Encoder Driver Module is to create abstracted functions to allow developers call upon this module to determine the speed of the wheel via Pulse Width and the distance that the wheel has travelled for by counting the number of splits within the rotary.

### Explanation

This library provides an interface to the encoder through a WYC-H206 encoder driver.

To determine the speed of the wheel using Pulse Width, time in microseconds is calculated for each pulse duration from when the the IR receiver is not blocked to when the IR receiver is blocked and unblocked again.

Based on the pulse duration, we are able to retrieve the amount of rotations per minute and derive the speed of the wheel by making use of the circumference of the rotary.

To determine the distance travelled, each time the IR receiver is unblocked, a counter is incremented. By making use of the length of each split, we can determine the distance travelled by taking the split in CM and multiplying it with the counter.

### Flowchart

-- To be included --

### Header Files

Definitions in `encoder_driver.h`:
```c

// GPIO Pin 2 to read the status of the Left Encoder
//
#define GPIO_PIN_ENC_LEFT 2

// GPIO Pin 3 to read the status of the Right Encoder
//
#define GPIO_PIN_ENC_RIGHT 3

// Number of Disc Slots the Rotary has
//
#define ENCODER_DISC_SLOTS 20

// Diameter of the Rotary
//
#define WHEEL_DIAMETER_CM 6.7

#define WHEEL_CIRCUMFERENCE_CM (M_PI * WHEEL_DIAMETER_CM)

// Distance in CM for each split of the Rotary
//
const float CM_PER_SLOT = WHEEL_CIRCUMFERENCE_CM / ENCODER_DISC_SLOTS;

struct repeating_timer timer;

// Variables used to keep track of the pulse duration of the Left Encoder
//
volatile uint32_t leftNewTime = 0;
volatile uint32_t leftOldTime = 0;
volatile uint32_t leftPulseTime = 0;

// Variables used to keep track of the pulse duration of the Right Encoder
//
volatile uint32_t rightNewTime = 0;
volatile uint32_t rightOldTime = 0;
volatile uint32_t rightPulseTime = 0;

// Counter to count the number of times the Left IR receiver is unblocked
//
volatile uint32_t left_interrupts = 0;

// Counter to count the number of times the Right IR Receiver is unblocked
//
volatile uint32_t right_interrupts = 0;

// Callback function to keep track of the pulse duration and number of times
// the IR receiver is unblocked
//
void gpio_callback_isr(uint gpio, uint32_t events);

// Calculates the number of full rotations (360 degrees) that the 
// rotary has made in terms of Rotations Per Minute
//
int get_wheel_rpm(uint gpio);

// Calculates the speed of the Left or Right Rotary in terms of km/h based on
// the Rotations Per Minute
//
int get_wheel_speed(uint gpio);

// Calculates the distance covered by the Left or Right rotary in cm based
// on the number of times the IR Received is unblocked
//
float get_wheel_distance(uint gpio);

// Function to initialize the GPIO Callback functions required
// for the encoder driver
//
void encoder_driver_init();
```