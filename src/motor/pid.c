#include "stdio.h"
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pid.h"

/*!
*	@brief	                    This function returns the duty cycle that the Left or Right
*                               Motor should be in order to accurately run at the specified
*                               speed
*   @param[in]  pid             Struct containing the values of the Left or Right Motor's PID
*                               structure
*   @param[in]  current_value   Contains the current speed detected retrieved from the Wheel
*                               encoder of the specified motor
*	@return					    Returns the duty cycle of the specified motor
*/
float compute_pid(PID *pid, float current_value)
{
    float error = pid->setpoint - current_value;

    // Integral
    //
    pid->i = pid->i + error;

    float derivative = error - pid->prev_error;

    float control_signal = pid->kp * error + pid->ki * pid->i + pid->kd * derivative;

    pid->prev_error = error;

    return control_signal;

    // // Proportional
    // //
    // pid->p = pid->kp * error;

    // // Integral
    // //
    // pid->i = pid->ki * error;

    // // Deriative
    // //
    // pid->d = pid->kd * (error - pid->prev_error);

    // float output = pid->p + pid->i + pid->d;

    // pid->prev_error = error;

    // return output;
}

/*!
*	@brief	                    This function creates a new PID structure
*   @param[in]  kp              Proportional Controller of the PID structure
*   @param[in]  ki              Integral Controller of the PID structure
*   @param[in]  kd              Derivative Controller of the PID structure
*   @param[in]  setpoint        Desired speed of the respective motor
*	@return					    Returns the a newly created PID structure
*/
PID *create_pid(float kp, float ki, float kd, float setpoint)
{
    PID *new_pid = malloc(sizeof(PID));

    if (new_pid != NULL)
    {
        new_pid->kp = kp;
        new_pid->ki = ki;
        new_pid->kd = kd;
        new_pid->p = 0;
        new_pid->i = 0;
        new_pid->d = 0;
        new_pid->setpoint = setpoint;
        new_pid->prev_error = 0.0;
    }

    return new_pid;
}

void setpoint_pid(PID *pid, int speed)
{
    pid->setpoint = speed;
}