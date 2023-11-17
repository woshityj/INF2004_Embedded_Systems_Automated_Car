#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "pico/cyw43_arch.h"
#include "lwip/ip4_addr.h"
#include "ping.h"

#include "encoder/encoder_driver.c"
#include "motor/motor_driver.c"
#include "motor/pid.c"


#include "pathfinder/algorithms.h"
#define CORE_ONE 0X2
#define CORE_ZERO 0x1
#define BOTH_CORES 0x3

#define mbaTASK_MESSAGE_BUFFER_SIZE       ( 60 )
#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif
#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

#define PID_Kp 2.f
#define PID_Ki 2.f
#define PID_Kd 0.f


void vLaunch( void) 
{
    /* Main task launch handler and initializers */

    // TaskHandle_t taskToMeasureTemperature;
    // xTaskCreate(main_task, "temperatureReaderThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &taskToMeasureTemperature);
    // vTaskCoreAffinitySet(taskToMeasureTemperature, BOTH_CORES);

    // TaskHandle_t taskToGetMovingAverage;
    // xTaskCreate(movingAverageTask, "movingAvgThread", configMINIMAL_STACK_SIZE, NULL, 7, & taskToGetMovingAverage);
    // vTaskCoreAffinitySet(taskToGetMovingAverage, CORE_ZERO);

    // TaskHandle_t taskToGetSimpleAverage;
    // xTaskCreate(simpleAverageTask, "simpleAverageThread", configMINIMAL_STACK_SIZE, NULL, 7, &taskToGetSimpleAverage);
    // vTaskCoreAffinitySet(taskToGetSimpleAverage, CORE_ZERO);

    // TaskHandle_t taskToPrintOutTemperatures;
    // xTaskCreate(printerTask, "printerThread", configMINIMAL_STACK_SIZE, NULL, 5, &taskToPrintOutTemperatures);
    // vTaskCoreAffinitySet(taskToPrintOutTemperatures, CORE_ONE);

    // TaskHandle_t wifiTask;
    // xTaskCreate(connectWifi, "wifi Connection", configMINIMAL_STACK_SIZE, NULL, 1, &wifiTask);
    // vTaskCoreAffinitySet(wifiTask, CORE_ONE);

    // temperatureToMovingTask = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    // temperatureToSimpleTask = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    // movingAverageBufferPrint = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    // simpleAverageBufferPrint = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);

    // UBaseType_t taskOne = vTaskCoreAffinityGet(taskToMeasureTemperature);
    // printf("Task One is on Core(s): %d\n", taskOne-1);

    // UBaseType_t taskTwo = vTaskCoreAffinityGet(taskToGetMovingAverage);
    // printf("Task Two is on Core: %d\n", taskTwo-1);
    
    // UBaseType_t taskThree = vTaskCoreAffinityGet(taskToGetSimpleAverage);
    // printf("Task Three is on Core: %d\n", taskThree-1);

    // UBaseType_t taskFour = vTaskCoreAffinityGet(taskToPrintOutTemperatures);
    // printf("Task Four is on Core: %d\n", taskFour-1);

    // UBaseType_t taskFive = vTaskCoreAffinityGet(wifiTask);
    // printf("Task Five is on Core: %d\n", taskFive-1);

//#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
//    vTaskCoreAffinitySet(main_task, 1);
//#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

typedef struct State
{

    uint left_motor_duty_cycle;
    uint right_motor_duty_cycle;

    float left_wheel_speed;
    float right_wheel_speed;

    PID *left_motor_pid;
    PID *right_motor_pid;
} State;

bool repeating_timer_callback_pid_isr(struct repeating_timer *t)
{
    State state = *((State *)t->user_data);
    
    state.left_motor_duty_cycle = compute_pid(state.left_motor_pid, state.left_wheel_speed);
    state.right_motor_duty_cycle = compute_pid(state.right_motor_pid, state.right_wheel_speed);

    if (state.left_motor_duty_cycle != get_speed(MOTOR_LEFT))
    {
        set_speed(state.left_motor_duty_cycle, MOTOR_LEFT);
    }

    if (state.right_motor_duty_cycle != get_speed(MOTOR_RIGHT))
    {
        set_speed(state.right_motor_duty_cycle, MOTOR_RIGHT);
    }

    printf("SP: %.2f | SPD L: %.2f | DUTY L: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", state.left_motor_pid->setpoint, state.left_wheel_speed, state.left_motor_duty_cycle, state.left_motor_pid->p, state.left_motor_pid->i, state.left_motor_pid->d, state.left_motor_pid->prev_error);
    printf("SP: %.2f | SPD R: %.2f | DUTY R: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", state.right_motor_pid->setpoint, state.right_wheel_speed, state.left_motor_duty_cycle, state.right_motor_pid->p, state.right_motor_pid->i, state.right_motor_pid->d, state.right_motor_pid->prev_error);

    return true;
}

int main( void )
{
    // Initialization
    stdio_usb_init();

    //     const char *rtos_name;
    // #if ( portSUPPORT_SMP == 1 )
    //     rtos_name = "FreeRTOS SMP";
    // #else
    //     rtos_name = "FreeRTOS";
    // #endif

    // #if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
    //     sleep_ms(5000);
    //     printf("Starting %s on both cores:\n", rtos_name);
    //     vLaunch();
    // #elif ( RUN_FREERTOS_ON_CORE == 1 )
    //     printf("Starting %s on core 1:\n", rtos_name);
    //     multicore_launch_core1(vLaunch);
    //     while (true);
    // #else
    //     printf("Starting %s on core 0:\n", rtos_name);
    //     vLaunch();
    // #endif
    //     return 0;

    State state;

    struct repeating_timer timer;

    state.left_motor_pid = create_pid(PID_Kp, PID_Ki, PID_Kd, 0);
    state.right_motor_pid = create_pid(PID_Kp, PID_Ki, PID_Kd, 0);

    motor_driver_init();
    encoder_driver_init();

    add_repeating_timer_ms(-200, repeating_timer_callback_pid_isr, &state, &timer);

    return 0;
}