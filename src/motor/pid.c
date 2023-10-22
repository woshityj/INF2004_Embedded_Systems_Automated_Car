#include "stdio.h"
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pid.h"

float compute_pid(pid_t *pid, float current_value, float *p_integral, float *p_prev_error)
{
    float error = pid->setpoint - current_value;

    *p_integral += error;

    float derivative = error - *p_prev_error;

    float control_signal = (pid->kp * error) + (pid->ki * ( *p_integral )) + (pid->kd * derivative);

    *p_prev_error = error;

    return control_signal;
}

pid_t create_pid(float kp, float ki, float kd, float setpoint)
{
    pid_t pid = {kp, ki, kd, setpoint};

    return pid;
}