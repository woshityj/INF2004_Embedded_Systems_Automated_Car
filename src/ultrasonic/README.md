## Ultrasonic Sensor Module

### Introduction

This sub-module library contains:

1. Ultrasonic Logic `ultrasonic.c`

### Objectives
The objective of the Ultrasonic Sensor is to provide the robotic car guidance in the form of object perception by using the Trigger Pin attached to the PicoW and the sensor to send a signal to the Ultrasonic Sensor to fire an ultrasound that travels through the air and bounces back to it, which then produces an echo pulse. fired back into the PicoW. Data processing can be done on the pulse length to determine if there is any object infront of the car or not.

### Explanation

This library provides an interface to the Ultrasonic Sensor.

To identify the surroundings of the robotic car, the Ultrasonic Sensor is placed at the front of the vehicle which will be used to identify if there are objects around the maze's path at a 30 degree angle infront of the sensor. 

-- Image to be included --

The software logic pulls the Trigger Pin attached on the PicoW up high, which also creates an alarm callback to pull the pin back down to signify the Trigger Pin on the sensor to notify the sensor to blast an ultrasound outwards. The pulse length is retrieved via interrupts, where the echo pin's edge rise signifies the start of the pulse and the edge fall signifies the end of the pulse. by taking the edge fall absolute timing and the invocation timing's difference, we are able to calculate the distance between the sensor and object by leveraging on the speed of sound in a medium, in our case, would be air.

The returned value is an unsigned short in millimeters so as to avoid any form of floating point arithmetic which is slow and inefficient and saves 16 bits of memory per short datatype, over the integer datatype

### Flowchart

-- To be included --

### Header Files

Definitions in `ultrasonic.h`:
```c

// GPIO 14 and 15 are being used respectively for the Trigger and Echo pin on the PicoW for GPIO_OUT and GPIO_IN
#define TRIGGER_PIN 14
#define ECHO_PIN 15

// The rate at which the Ultrasonic Sensor will sample a simple average in before passing it back to the task that calls the sensor
#define SAMPLING_RATE 30

// If the pulse takes too long to reach back, it is considered too far to be of use, and generally out of range
#define TIMEOUT 26100

// Function declarations

// Helper function to initialize the ultrasonic sensor related pins
void initializeUltrasonic(unsigned char triggerPin, unsigned char echoPin);

// The Echo Pin callback function when it experiences and interrupt from the echo pin
void echo_interrupt(uint gpio, uint32_t events);

// The alarm that is set to pull the trigger pin back down to let the ultrasonic sensor send the ultrasound
int64_t alarm_pulldown_callback(alarm_id_t id, void *user_data);

// A repeating timer that continuously calls the alarm above (LINE 52) every so often
bool pullTrigger(struct repeating_timer *t);

// A helper function that computes the Pulse length and converts it into millimeters
unsigned short getMM();

// Global variables used in the interrupts
absolute_time_t startTime;
absolute_time_t endTime;
absolute_time_t pulseLength;

```