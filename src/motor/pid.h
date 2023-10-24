typedef struct {
    float kp;
    float ki;
    float kd;

    float setpoint;
} pid_t;

float compute_pid(pid_t *pid, float current_value, float *p_integral, float *p_prev_error);
pid_t create_pid(float kp, float ki, float kd, float setpoint);
