#ifndef ultrasonic
#define ultrasonic

#define TRIGGER_PIN 14
#define ECHO_PIN 15
#define SAMPLING_RATE 30
#define ZERO_THRESHOLD 20 // 66% of sampling_rate
#define DISTANCE_THRESHOLD 300 // in MM, 300mm == 30cm
#define TIMEOUT 26100

#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

void initializeUltrasonic(unsigned char triggerPin, unsigned char echoPin);
void echo_interrupt(uint gpio, uint32_t events);
bool pullTrigger(struct repeating_timer *t);
unsigned short getMM();
int64_t alarm_pulldown_callback(alarm_id_t id, void *user_data);
bool getObstacle();

absolute_time_t startTime;
absolute_time_t endTime;
absolute_time_t pulseLength;

#endif