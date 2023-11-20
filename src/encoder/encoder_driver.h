#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/time.h"

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

int cm_to_interrupts(int cm);

bool timer_callback_isr_alert(struct repeating_timer *t);

void encoder_alert_after_isr_interrupt(uint target, repeating_timer_callback_t callback);

#endif