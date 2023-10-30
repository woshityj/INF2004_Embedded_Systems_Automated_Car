#ifndef PID_H_
#define PID_H_

#include "stdio.h"
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

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

#endif