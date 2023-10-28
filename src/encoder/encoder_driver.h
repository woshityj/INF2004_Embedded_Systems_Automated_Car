#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#define GPIO_PIN_ENC_LEFT 2
#define GPIO_PIN_ENC_RIGHT 3

#define INTERRUPT_BUF_SIZE 5

#define ENCODER_DISC_SLOTS 20
#define WHEEL_DIAMETER_CM 6.7
#define WHEEL_CIRCUMFERENCE_CM (M_PI * WHEEL_DIAMETER_CM)

const float CM_PER_SLOT = WHEEL_CIRCUMFERENCE_CM / ENCODER_DISC_SLOTS;

void gpio_callback_isr(uint gpio, uint32_t events);
int get_wheel_rpm(uint gpio);
int get_wheel_speed(uint gpio);
float get_wheel_distance(uint gpio);
void encoder_driver_init();

#endif