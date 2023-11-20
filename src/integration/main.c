#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#include "../encoder/encoder_driver.c"
#include "../motor/motor_driver.c"
#include "../motor/pid.c"
#include "../ultrasonic/ultrasonic.h"
#include "../ultrasonic/ultrasonic.c"
#include "../infrared/infrared.c"

#define PID_Kp 2.f
#define PID_Ki 2.f
#define PID_Kd 0.f

#define MIN_SPEED 5         // In terms of KM/h
#define MAX_SPEED 15        // In terms of KM/h

#define GRID_SIZE 15        // In terms of cm

#define MIN_TURN_ANGLE 45   // Minimum turning angle is 45 degrees

#define MOTOR_TURN_CLOCKWISE 0
#define MOTOR_TURN_ANTICLOCKWISE 1

// #define PID_TASK_PRIORITY               ( tskIDLE_PRIORITY + 5UL )
// #define WHEEL_SPEED_TASK_PRIORITY       ( tskIDLE_PRIORITY + 3UL )
// #define ULTRASONIC_TASK_PRIORITY        ( tskIDLE_PRIORITY + 1UL )

struct repeating_timer distance_timer;

bool distance_completed = false;
volatile bool object_detected = false;
volatile bool object_detection_completed = false;

typedef struct State
{
    uint left_motor_duty_cycle;
    uint right_motor_duty_cycle;

    float left_wheel_speed;
    float right_wheel_speed;

    PID *left_motor_pid;
    PID *right_motor_pid;
    repeating_timer_t pid_timer;
    repeating_timer_t wheel_speed_timer;
} State;

State state;

void move_forward_with_distance(int cm, int speed)
{
    int no_of_interrupts = cm_to_interrupts(cm);

    distance_completed = false;

    left_interrupts = 0;
    right_interrupts = 0;

    MOTOR_move_forward();
    PID_setpoint(state.left_motor_pid, speed);
    PID_setpoint(state.right_motor_pid, speed);
    
    while (!distance_completed)
    {
        if ((left_interrupts >= no_of_interrupts) || (right_interrupts >= no_of_interrupts))
        {
            distance_completed = true;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    PID_setpoint(state.left_motor_pid, 0);
    PID_setpoint(state.right_motor_pid, 0);

    printf("[Motor & Encoder] Moved Forward %dcm\n", cm);

    return;
}

void move_backward_with_distance(int cm, int speed)
{
    int no_of_interrupts = cm_to_interrupts(cm);

    distance_completed = false;

    left_interrupts = 0;
    right_interrupts = 0;

    MOTOR_move_backward();
    
    PID_setpoint(state.left_motor_pid, speed);
    PID_setpoint(state.right_motor_pid, speed);

    while (!distance_completed)
    {

        if ((left_interrupts >= no_of_interrupts) || (right_interrupts >= no_of_interrupts))
        {
            distance_completed = true;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    PID_setpoint(state.left_motor_pid, 0);
    PID_setpoint(state.right_motor_pid, 0);

    printf("[Motor & Encoder] Moved Backwards %dcm\n", cm);

    return;
}

void spot_turn_pid(int turn_direction, int angle)
{
    if (angle < MIN_TURN_ANGLE)
    {
        angle = MIN_TURN_ANGLE;
    }

    // Ensure angle is a multiple of MIN_TURN_ANGLE
    int angle_error = angle % MIN_TURN_ANGLE;
    if (angle_error > 0)
    {
        if (angle_error > MIN_TURN_ANGLE / 2)
        {
            angle += MIN_TURN_ANGLE - angle_error;
        }
        else
        {
            angle -= angle_error;
        }
    }

    int interrupts = 4 * angle / MIN_TURN_ANGLE;

    if (turn_direction == MOTOR_TURN_CLOCKWISE)
    {
        MOTOR_turn_right();
    }
    else if (turn_direction == MOTOR_TURN_ANTICLOCKWISE)
    {
        MOTOR_turn_left();
    }

    PID_setpoint(state.left_motor_pid, 5);
    PID_setpoint(state.right_motor_pid, 5);
    encoder_alert_after_isr_interrupt(interrupts, pid_stop_callback);
}

// void wall_detected()
// {
//     PID_setpoint(state.left_motor_pid, 0);
//     PID_setpoint(state.right_motor_pid, 0);

//     move_backward_with_distance(8, 10);
//     spot_turn_pid
// }

// void barcode_scanning_mode(State *state)
// {
//     // Remove backwards half a grid
//     move_backward_with_distance(GRID_SIZE / 2, 10, &state);

//     // Set to move forward at a constant low speed
//     PID_setpoint(&state->left_motor_pid, 5);
//     PID_setpoint(&state->right_motor_pid, 5);

//     // Initialize Front IR Sensor scan for barcode
//     IR_barcode_scan(&barcode_timer);

//     return;
// }

// Function is called when an object is detected
//


void pid_task(__unused void *params)
{
    printf("[PID] Initializing PID in FreeRTOS");

    // add_repeating_timer_ms(-10, repeating_timer_callback_wheel_speed_isr, &state, &state.wheel_speed_timer);

    while (true)
    {
        state.left_motor_duty_cycle = PID_compute(state.left_motor_pid, state.left_wheel_speed);
        state.right_motor_duty_cycle = PID_compute(state.right_motor_pid, state.right_wheel_speed);

        if (state.left_motor_duty_cycle != MOTOR_get_speed(MOTOR_LEFT))
        {
            MOTOR_set_speed(state.left_motor_duty_cycle, MOTOR_LEFT);
        }

        if (state.right_motor_duty_cycle != MOTOR_get_speed(MOTOR_RIGHT))
        {
            MOTOR_set_speed(state.right_motor_duty_cycle, MOTOR_RIGHT);
        }

        printf("SP: %.2f | SPD L: %.2f | DUTY L: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", state.left_motor_pid->setpoint, state.left_wheel_speed, state.left_motor_duty_cycle, state.left_motor_pid->p, state.left_motor_pid->i, state.left_motor_pid->d, state.left_motor_pid->prev_error);
        printf("SP: %.2f | SPD R: %.2f | DUTY R: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", state.right_motor_pid->setpoint, state.right_wheel_speed, state.right_motor_duty_cycle, state.right_motor_pid->p, state.right_motor_pid->i, state.right_motor_pid->d, state.right_motor_pid->prev_error);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void ultrasonic_task(__unused void *params)
{
    printf("[Ultrasonic] Initializing Ultrasonic in FreeRTOS");

    initializeUltrasonic();

    while (true)
    {
        if (getObstacle())
        {
            object_detected = true;
            // printf("[WARNING] Obstacle Detected! Mother die\n");
        }
        vTaskDelay(1);
    }
}

void encoder_task(__unused void *params)
{
    printf("[Encoder] Initializing Encoder in FreeRTOS");

    while (true)
    {
        float left_speed = get_wheel_speed(GPIO_PIN_ENC_LEFT);
        float right_speed = get_wheel_speed(GPIO_PIN_ENC_RIGHT);
        state.left_wheel_speed = left_speed;
        state.right_wheel_speed = right_speed;
        // printf("[Left Wheel Speed] %d\n", left_interrupts);
        vTaskDelay(1);
    }
}


void main_task(__unused void *params)
{
    printf("[Main Task] Initializing Main Task in FreeRTOS");

    while (true)
    {
        if (!object_detected && object_detection_completed == false)
        {
            MOTOR_move_forward();

            PID_setpoint(state.left_motor_pid, 10);
            PID_setpoint(state.right_motor_pid, 10);
        }
        else if (object_detected && object_detection_completed == false)
        {
            PID_setpoint(state.left_motor_pid, 0);
            PID_setpoint(state.right_motor_pid, 0);
            move_backward_with_distance(20, 10);

            object_detected = false;
            object_detection_completed = true;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

}

void vLaunch(void)
{
    TaskHandle_t ultrasonicTask;
    xTaskCreate(ultrasonic_task, "Ultrasonic Sensor", configMINIMAL_STACK_SIZE, NULL, 2, &ultrasonicTask);

    TaskHandle_t pidTask;
    xTaskCreate(pid_task, "PID Task", configMINIMAL_STACK_SIZE, NULL, 2, &pidTask);

    TaskHandle_t encoderTask;
    xTaskCreate(encoder_task, "Encoder Sensor", configMINIMAL_STACK_SIZE, NULL, 2, &encoderTask);

    TaskHandle_t mainTask;
    xTaskCreate(main_task, "Main Task", configMINIMAL_STACK_SIZE, NULL, 4, &mainTask);

    vTaskStartScheduler();
}


int main (void)
{
    // Initialization
    stdio_init_all();

    // Mode 1 means mapping mode
    // Mode 2 means barcode scanning mode
    //
    int mode = 1;

    state.left_motor_pid = PID_create(PID_Kp, PID_Ki, PID_Kd, 0);
    state.right_motor_pid = PID_create(PID_Kp, PID_Ki, PID_Kd, 0);

    encoder_driver_init();
    MOTOR_driver_init(state.left_motor_pid, state.right_motor_pid);

    // MOTOR_move_forward();
    // PID_setpoint(state.left_motor_pid, 10);
    // PID_setpoint(state.right_motor_pid, 10);
    // move_forward_with_distance(10, 10);
    // spot_turn_pid(0, 90);

    vLaunch();


    // move_forward_with_distance(50, 10, &state);

    // spot_turn_pid(state.left_motor_pid, state.right_motor_pid, MOTOR_TURN_CLOCKWISE, 90);

    // spot_turn_pid(state.left_motor_pid, state.right_motor_pid, 0, 90);

    // while(1)
    // {
    //     switch(mode)
    //     {
    //         case 1:
    //             // If condition to check if front has barcode or wall
    //             // While there is no wall, move forward a grid
    //             move_forward_with_distance(GRID_SIZE, 5, &state);
    //             break;

    //         // Barcode Scanning Mode
    //         case 2:
    //             struct repeating_timer barcode_timer;
    //             move_backward_with_distance(GRID_SIZE / 2, 4, &state);
    //             add_repeating_timer_ms(SAMPLE_RATE_MS, IR_barcode_scan, NULL, &barcode_timer);
    //             MOTOR_move_forward();
    //             PID_setpoint(state.left_motor_pid, 4);
    //             PID_setpoint(state.left_motor_pid, 4);
    //             // Set IR Sensor to Barcode Scanning Mode
    //             // while (condition where barcode scanning mode not completed)
    //             // {
    //             //     sleep_ms(10);
    //             // }
            
    //             break;
                   

            
    //         default:
    //             break;
    //     }
    // }

    return 0;
}