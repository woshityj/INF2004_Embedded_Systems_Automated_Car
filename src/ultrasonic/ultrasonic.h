#ifndef ultrasonic
#define ultrasonic

#define TRIGGER_PIN 14
#define ECHO_PIN 15
#define SAMPLING_RATE 10
#define ZERO_THRESHOLD 7 // 66% of sampling_rate
#define DISTANCE_THRESHOLD 200 // in MM, 300mm == 30cm
#define TIMEOUT 26100
#define DIFFERENCE(a, b) (a<b ? (b-a) : (a-b))

#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

void initializeUltrasonic();
void echo_interrupt(uint gpio, uint32_t events);
bool pullTrigger(struct repeating_timer *t);
unsigned short getMM();
int64_t alarm_pulldown_callback(alarm_id_t id, void *user_data);
bool getObstacle();

int64_t startTime;
int64_t endTime;
int64_t pulseLength;

#endif