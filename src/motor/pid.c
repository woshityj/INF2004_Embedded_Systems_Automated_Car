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
float compute_pid(pid_t *pid, float current_value)
{
    float error = pid->setpoint - current_value;

    pid->integral += error;

    float derivative = error - pid->prev_error;

    float control_signal = (pid->kp * error) + (pid->ki * ( pid->integral )) + (pid->kd * derivative);

    pid->prev_error = error;

    return control_signal;
}

/*!
*	@brief	                    This function creates a new PID structure
*   @param[in]  kp              Proportional Controller of the PID structure
*   @param[in]  ki              Integral Controller of the PID structure
*   @param[in]  kd              Derivative Controller of the PID structure
*   @param[in]  setpoint        Desired speed of the respective motor
*	@return					    Returns the a newly created PID structure
*/
pid_t create_pid(float kp, float ki, float kd, float setpoint)
{
    pid_t pid = {kp, ki, kd, setpoint};
    pid.integral = 0;
    pid.prev_error = 0;

    return pid;
}