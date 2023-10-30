## Motor Driver Module

### Introduction

This sub-module library contains:

1. Motor control logic `motor_driver.c`
2. PID Controller `pid.c`

### Objectives
The objective of the L293N Motor Driver Module is to create abstracted functions to allow developers call upon this module to control the direction in which the car will move via the motors and control the speed of the motors via Pulse Width Modulation (PWM)

### Explanation

This library provides an interface to the motors through a L298N motor driver.

A PID controller is implemented to control the speed of motors. Each PID controller instance is designed to control a single motor.

The PID feedback loop takes in a target wheel speed (km/h) and outputs the PWM duty cycle. The feedback loop is depicted in the diagram below:

```
SetPoint        (wheel speed) ↘         -> P ↘
CurrentValue    (wheel speed) -> Error  -> I -> Controller -> Duty Cycle (0%-100%)
                                        -> D ↗
```

### Flowchart

-- To be included --

### Header Files

Definitions in `motor.h`:
```c

// GPIO Pin 0 to Control Pulse Width Modulation of Left Motor
//
#define GPIO_PIN_PWM_EN1 0

// GPIO Pin 1 to Control Pulse Width Modulation of Right Motor
//
#define GPIO_PIN_PWM_EN2 1

// GPIO Pins 19 and 18 to control the direction where the Left Motor
// will spin in
//
#define GPIO_PIN_MOTOR_IN1 19
#define GPIO_PIN_MOTOR_IN2 18

// GPIO Pins 17 and 16 to control the direction where the Right Motor
// will spin in
//
#define GPIO_PIN_MOTOR_IN3 17
#define GPIO_PIN_MOTOR_IN4 16

// Preprocessor to easily call upon the Left or Right Motor
//
#define MOTOR_LEFT 2
#define MOTOR_RIGHT 1

// Used in setting the duty cycle of the Left or Right Motor
//
#define PWM_CYCLE 12500

// Function to initialize the Motor Driver
//
void motor_driver_init()

// Function to set the direction of the Left and Right Motors
// to move forward
//
void move_forward();

// Function to set the direction of the Left and Right Motors
// to move backwards
//
void move_backward();

// Function to set the direction of the Left and Right Motors
// to turn left
//
void turn_left();

// Function to set the direction of the Left and Right Motors
// to turn right
//
void turn_right();

// Function to set the Left or Right Motor's PWM duty cycle
//
void set_speed(uint duty_cycle, int motor);

// Function to retrieve the speed of the Left or Right Motor in
// terms its duty cycle in percentage
//
int get_speed(int motor);

// Function to stop both Left and Right Motor
//
void stop();
```

Definitions in `pid.h`:
```c
// Struct used in creation of a new PID Controller structure
// for each Motor
//
typedef struct {
    float kp;
    float ki;
    float kd;

    float setpoint;
    float integral, prev_error;
} pid_t;

// Function used to create a new PID Controller structure
//
pid_t create_pid(float kp, float ki, float kd, float setpoint);

// Function used to return the duty cycle in percentage (0 - 100) for the repsective motor
//
float compute_pid(pid_t *pid, float current_value);


```