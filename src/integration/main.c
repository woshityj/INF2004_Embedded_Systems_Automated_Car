#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#include "../encoder/encoder_driver.c"
#include "../motor/motor_driver.c"
#include "../motor/pid.c"
#include "../ultrasonic/ultrasonic.h"
#include "../ultrasonic/ultrasonic.c"
#include "../infrared/infrared.c"
#include "../magnetometer/magnetometer_driver.c"

#include "ssi.h"
#include "cgi.h"
#include "lwipopts.h"
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"

#define PID_Kp 2.f
#define PID_Ki 2.f
#define PID_Kd 0.f

#define MIN_SPEED 5         // In terms of KM/h
#define MAX_SPEED 15        // In terms of KM/h

#define GRID_SIZE 15        // In terms of cm

#define MIN_TURN_ANGLE 45   // Minimum turning angle is 45 degrees

#define MOTOR_TURN_CLOCKWISE 0
#define MOTOR_TURN_ANTICLOCKWISE 1

#define I2C_BAUD 400 // I2C baud rate (400 kHz)
#define REFRESH_PERIOD 100 // Refresh period in milliseconds


// #define PID_TASK_PRIORITY               ( tskIDLE_PRIORITY + 5UL )
// #define WHEEL_SPEED_TASK_PRIORITY       ( tskIDLE_PRIORITY + 3UL )
// #define ULTRASONIC_TASK_PRIORITY        ( tskIDLE_PRIORITY + 1UL )

/* Global Variables to initialize */

mag_t mag;
struct repeating_timer distance_timer;

bool distance_completed = false;
volatile bool object_detected = false;
volatile bool object_detection_completed = false;
volatile int starting_angle = 0;

char barcode_char;
const char * ssi_tags[] = {"barcode"};

/* Struct definitions */
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
/* Global Variables Ending*/

/* Function prototypes start*/
u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen);
void ssi_init();
void move_forward_with_distance(int cm, int speed);
void move_backward_with_distance(int cm, int speed);
void spot_turn_pid(int turn_direction, int angle);
void spot_turn_pid_interrupts(int turn_direction, int interrupts);
void wall_detected_action();
void barcode_scanning_mode(State *state);
void pid_task(__unused void *params);
void ultrasonic_task(__unused void *params);
void encoder_task(__unused void *params);
void main_task(__unused void *params);
void left_right_ir_task(__unused void *params);
void barcode_scanning_task();
void webserver_task(__unused void *params);

/* Function prototypes end here*/



u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) 
{
  size_t printed;

  switch(iIndex)
  {
    case 0: // Barcode
    {
        printed = snprintf(pcInsert, iInsertLen, "%c", barcode_char);
        printf("Testing\n");
        break;
    }
    default:
        printed = 0;
        break;
  }

  return (u16_t)printed;
}

// Initialise the SSI handler
void ssi_init()
{
  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}

/*!
*	@brief	            This function moves the car forward at a specified distance in cm and speed
*   @param[in]  cm      Distance the car should travel in cm
*   @param[in]  speed   Speed in terms of km/h that the car would travel the distance via
*/
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

/*!
*	@brief	            This function moves the car backwards at a specified distance in cm and speed
*   @param[in]  cm      Distance the car should travel in cm
*   @param[in]  speed   Speed in terms of km/h that the car would travel the distance via
*/
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

/*!
*	@brief	                        This function turns the car in a specific direction at a specificied angle 
*   @param[in]  turn_direction      Direction for the car to turn in, Clockwise or Anti-Clockwise
*   @param[in]  angle               Angle that the car would turn and stop at
*/
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

    left_interrupts_isr_counter = 0;
    right_interrupts_isr_counter = 0;
    left_interrupts_isr_target_reached = 0;
    right_interrupts_isr_target_reached = 0;
    isr_target = interrupts;
    enable_isr_counter = 1;

    PID_setpoint(state.left_motor_pid, 5);
    PID_setpoint(state.right_motor_pid, 5);

    while (((left_interrupts_isr_target_reached != 1) && (right_interrupts_isr_target_reached != 1)))
    {
        vTaskDelay(pdMS_TO_TICKS(2));
    }

    PID_setpoint(state.left_motor_pid, 0);
    PID_setpoint(state.right_motor_pid, 0);
    MOTOR_set_speed(0, MOTOR_LEFT);
    MOTOR_set_speed(0, MOTOR_RIGHT);
    enable_isr_counter = 0;


    printf("[Encoder] Successfully turned\n");
}

/*!
*	@brief	                        This function turns the car in a specific direction at a specified number
*                                   of interrupts based on the number of slits from the wheel encoder
*   @param[in]  turn_direction      Direction for the car to turn in, Clockwise or Anti-Clockwise
*   @param[in]  interrupts          Number of interrupts / slits from the wheel encoder before the car stops turning
*/
void spot_turn_pid_interrupts(int turn_direction, int interrupts)
{
    if (MOTOR_TURN_CLOCKWISE == turn_direction)
    {
        MOTOR_turn_right();
    }
    else if (MOTOR_TURN_ANTICLOCKWISE == turn_direction)
    {
        MOTOR_turn_left();
    }

    left_interrupts_isr_counter = 0;
    right_interrupts_isr_counter = 0;
    left_interrupts_isr_target_reached = 0;
    right_interrupts_isr_target_reached = 0;
    isr_target = interrupts;
    enable_isr_counter = 1;

    PID_setpoint(state.left_motor_pid, 5);
    PID_setpoint(state.right_motor_pid, 5);

    while (((left_interrupts_isr_target_reached != 1) && (right_interrupts_isr_target_reached != 1)))
    {
        vTaskDelay(pdMS_TO_TICKS(2));
    }

    PID_setpoint(state.left_motor_pid, 0);
    PID_setpoint(state.right_motor_pid, 0);
    MOTOR_set_speed(0, MOTOR_LEFT);
    MOTOR_set_speed(0, MOTOR_RIGHT);
    enable_isr_counter = 0;


    printf("[Encoder] Successfully turned\n");
}

/*!
*	@brief              This function performs the necessary action when a wall is detected from the
*                       ultrasonic sensor, where the car would stop and move back half a grid's lengh
*                       and turn in a clockwise direction for 90 degrees 
*/
void wall_detected_action()
{
    PID_setpoint(state.left_motor_pid, 0);
    PID_setpoint(state.right_motor_pid, 0);
    MOTOR_set_speed(0, MOTOR_LEFT);
    MOTOR_set_speed(0, MOTOR_RIGHT);

    move_backward_with_distance((GRID_SIZE / 2), 10);

    vTaskDelay(pdMS_TO_TICKS(1000));
    spot_turn_pid(MOTOR_TURN_CLOCKWISE, 90);
}

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

/*!
*	@brief              This function helps maintain a consistent speed for both left and right
*                       motors in terms of km/h using PID
*/
void pid_task(__unused void *params)
{
    printf("[PID] Initializing PID in FreeRTOS");

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

        // printf("SP: %.2f | SPD L: %.2f | DUTY L: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", state.left_motor_pid->setpoint, state.left_wheel_speed, state.left_motor_duty_cycle, state.left_motor_pid->p, state.left_motor_pid->i, state.left_motor_pid->d, state.left_motor_pid->prev_error);
        // printf("SP: %.2f | SPD R: %.2f | DUTY R: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", state.right_motor_pid->setpoint, state.right_wheel_speed, state.right_motor_duty_cycle, state.right_motor_pid->p, state.right_motor_pid->i, state.right_motor_pid->d, state.right_motor_pid->prev_error);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/*!
*	@brief              This FreeRTOS tasks initializes the ultrasonic sensor and
*                       constantly detects if there is an obstacle within 20cm
*/
void ultrasonic_task(__unused void *params)
{
    printf("[Ultrasonic] Initializing Ultrasonic in FreeRTOS");

    initializeUltrasonic();

    while (true)
    {
        if (getObstacle())
        {
            object_detected = true;
            printf("[WARNING] Obstacle Detected! Mother die\n");
        }
        vTaskDelay(1);
    }
}

/*!
*	@brief              This FreeRTOS tasks initializes the wheel encoder and
*                       constantly tracks the current speed of the car in terms of
*                       km/h, which will be used by the PID function to maintain
*                       the speed of the car at the desired or set speed
*/
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

/*!
*	@brief              This FreeRTOS tasks contains the functions required for the car
*                       to operate
*/
void main_task(__unused void *params)
{
    printf("[Main Task] Initializing Main Task in FreeRTOS");

    adc_select_input(ADC_FRONT);

    MOTOR_move_forward();
    PID_setpoint(state.left_motor_pid, 5);
    PID_setpoint(state.right_motor_pid, 5);

    while (true)
    {
        // Speed Function for Barcode Scanning Mode
        //
        // PID_setpoint(state.left_motor_pid, 4);
        // PID_setpoint(state.right_motor_pid, 4);

        // Speed Function for straight line test
        //
        // PID_setpoint(state.left_motor_pid, 5);
        // PID_setpoint(state.right_motor_pid, 5);

        // IR Sensor Function for Wall Detection and Turn
        
        int value = adc_read();

        printf("%d\n", value);
        if (!object_detected && !object_detection_completed)
        {
            if (value >= 1500)
            {
                printf("[Infrared] Wall Detected\n");
                PID_setpoint(state.left_motor_pid, 0);
                PID_setpoint(state.right_motor_pid, 0);
                MOTOR_set_speed(0, MOTOR_LEFT);
                MOTOR_set_speed(0, MOTOR_RIGHT);
                object_detection_completed = true;


                vTaskDelay(pdMS_TO_TICKS(1000));
                move_backward_with_distance(10, 5);

                vTaskDelay(pdMS_TO_TICKS(1000));

                spot_turn_pid(MOTOR_TURN_CLOCKWISE, 90);

                vTaskDelay(pdMS_TO_TICKS(1000));
                move_forward_with_distance(10, 4);
            }
        }

        // Function for Ultrasonic Wall Detection
        //
        if (!object_detected && object_detection_completed == false)
        {
            MOTOR_move_forward();

            PID_setpoint(state.left_motor_pid, 8);
            PID_setpoint(state.right_motor_pid, 8);
        }
        else if (object_detected && false == object_detection_completed)
        {
            PID_setpoint(state.left_motor_pid, 0);
            PID_setpoint(state.right_motor_pid, 0);
            move_backward_with_distance(20, 8);

            object_detected = false;
            object_detection_completed = true;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/*!
*	@brief              This function makes use of the left and right IR sensors
*                       to ensure that whenever a wall is detected on either the 
*                       left or the right IR sensor, it will turn clockwise or
*                       anti-clockwise respectively to ensure that the car
*                       maintains within the confinement of the walls
*/
void left_right_ir_task(__unused void *params)
{
    printf("[IR] Initializing Left and Right IR Sensors in FreeRTOS");
    vTaskDelay(pdMS_TO_TICKS(2000));

    while (true)
    {
        bool left_wall_detected = detect_line(ADC_LEFT);
        bool right_wall_detected = detect_line(ADC_RIGHT);

        printf("Left Wall Value: %d \n", left_wall_detected);
        printf("Right Wall Value: %d \n", right_wall_detected);

        // If Left Wall detected but Right Wall is not detected
        if (left_wall_detected == 1 && right_wall_detected == 0)
        {
            // Turn Right
            spot_turn_pid_interrupts(MOTOR_TURN_CLOCKWISE, 1);
            printf("[IR] Turning Left\n");
        }
        // If Right Wall detected and Left Wall is not detected
        else if (right_wall_detected == 1 && left_wall_detected == 0)
        {
            // Turn Left
            spot_turn_pid_interrupts(MOTOR_TURN_ANTICLOCKWISE, 1);
            printf("[IR] Turning Right\n");
        }
        else if (left_wall_detected == 1 && right_wall_detected == 1)
        {
            move_forward_with_distance(2, 10);
        }
        else
        {
            move_forward_with_distance(2, 10);
        }

        vTaskDelay(pdMS_TO_TICKS(75));
    }
}

/*!
*	@brief              This function performs the barcode scanning task through the front
*                       IR sensor and prints out the character
*/
void barcode_scanning_task()
{
    printf("[IR] Initializing Barcode Scanning in FreeRTOS");

    adc_select_input(ADC_FRONT);
    while (true)
    {
        // int value = adc_read();
        // printf("%d\n", value);
        char temp_char = IR_barcode_scan();
        if (temp_char != '?')
        {
            barcode_char = temp_char;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/*!
*	@brief              This function starts the webserver and prints out the barcode
*                       character onto the web server
*/
void webserver_task(__unused void *params)
{
    vTaskDelay(pdMS_TO_TICKS(10000));
    // Connect to the WiFI network - loop until connected
    while(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0)
    {
        printf("Attempting to connect...\n");
    }
    // Print a success message once connected
    printf("Connected! \n");
    
    // Initialise web server
    httpd_init();
    printf("Http server initialised\n");

    // Configure SSI and CGI handler
    ssi_init(); 
    printf("SSI Handler initialised\n");
    cgi_init();
    printf("CGI Handler initialised\n");

    printf("IP: %s\n", ip4addr_ntoa(netif_ip_addr4(netif_default)));
    
    while (true)
    {

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
    xTaskCreate(main_task, "Main Task", configMINIMAL_STACK_SIZE, NULL, 2, &mainTask);

    TaskHandle_t leftRightIRTask;
    xTaskCreate(left_right_ir_task, "Left Right IR Task", configMINIMAL_STACK_SIZE, NULL, 4, &leftRightIRTask);

    TaskHandle_t barcodeScanningTask;
    xTaskCreate(barcode_scanning_task, "Barcode Scanning Task", configMINIMAL_STACK_SIZE, NULL, 6, &barcodeScanningTask);

    TaskHandle_t webserverTask;
    xTaskCreate(webserver_task, "Webserver Task", configMINIMAL_STACK_SIZE, NULL, 6, &webserverTask);

    vTaskStartScheduler();
}


int main (void)
{
    // Initialization
    stdio_init_all();

    cyw43_arch_init();

    cyw43_arch_enable_sta_mode();

    // Mode 1 means mapping mode
    // Mode 2 means barcode scanning mode
    //
    int mode = 1;

    state.left_motor_pid = PID_create(PID_Kp, PID_Ki, PID_Kd, 0);
    state.right_motor_pid = PID_create(PID_Kp, PID_Ki, PID_Kd, 0);

    encoder_driver_init();
    MOTOR_driver_init(state.left_motor_pid, state.right_motor_pid);
    IR_init();

    // Function for Barcode Scanning
    //
    MOTOR_move_forward();
    PID_setpoint(state.left_motor_pid, 3);
    PID_setpoint(state.right_motor_pid, 3);

    // PID_setpoint(state.left_motor_pid, 3);
    // PID_setpoint(state.right_motor_pid, 3);


    // Ultrasonic Sensor Setup
    // init_i2c_default();
    // create_mag_setup();

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