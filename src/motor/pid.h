#ifndef PID_H_
#define PID_H_

#include "stdio.h"
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

// Struct used in creation of a new PID Controller structure
// for each Motor
//
typedef struct _PID {
    float kp;
    float ki;
    float kd;

    float p;
    float i;
    float d;

    float setpoint;
    float prev_error;
} PID;

// Function used to create a new PID Controller structure
//
PID* create_pid(float kp, float ki, float kd, float setpoint);

// Function used to return the duty cycle in percentage (0 - 100) for the repsective motor
//
float compute_pid(PID *pid, float current_value);

#endif