// Logic Error -- Adjusted PID Controller parameters to suit the
// ones shown in the pseudocode
//

typedef struct {
    float kp;
    float ki;
    float kd;

    float setpoint;
} pid_t;

float create_pid(float kp, float ki, float kd, float setpoint);
float compute_pid(float setpoint, float current_value, float *p_integral, float *p_prev_error);
