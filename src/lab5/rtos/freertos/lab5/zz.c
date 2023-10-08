/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
//helloworld
#include <stdio.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "lwip/ip4_addr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"
#include "message_buffer.h"

#include "hardware/gpio.h"
#include "hardware/adc.h"

#define mbaTASK_MESSAGE_BUFFER_SIZE       ( 60 )

#ifndef PING_ADDR
#define PING_ADDR "142.251.35.196"
#endif
#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )
#define MAX_TEMP_POINTS 10

//static MessageBufferHandle_t xControlMessageBuffer;
static MessageBufferHandle_t temperatureToMovingTask;
static MessageBufferHandle_t temperatureToSimpleTask;
static MessageBufferHandle_t movingAverageBufferPrint;
static MessageBufferHandle_t simpleAverageBufferPrint;

float read_onboard_temperature() {
    
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    return tempC;
}

void main_task(__unused void *params) {
    
    float temperature = 0.0;

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    while(true) {
        vTaskDelay(1000);
        temperature = read_onboard_temperature();
        //printf("Onboard temperature = %.02f C\n", temperature);

        xMessageBufferSend( 
            temperatureToMovingTask,    /* The message buffer to write to. */
            (void *) &temperature,    /* The source of the data to send. */
            sizeof( temperature ),    /* The length of the data to send. */
            0 );                      /* Do not block, should the buffer be full. */
        
        xMessageBufferSend( 
            temperatureToSimpleTask,    /* The message buffer to write to. */
            (void *) &temperature,    /* The source of the data to send. */
            sizeof( temperature ),    /* The length of the data to send. */
            0 );                      /* Do not block, should the buffer be full. */
    }
}

///* A Task that blinks the LED for 3000 ticks continuously */
//void led_task(__unused void *params) {
//    while(true) {
//        vTaskDelay(3000);
//        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
//        vTaskDelay(3000);
//        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
//    }
//}

/* A Task that obtains the data every 1000 ticks from the inbuilt temperature sensor (RP2040), prints it out and sends it to avg_task via message buffer */
//void temp_task(__unused void *params) {
//    float temperature = 0.0;
//
//    adc_init();
//    adc_set_temp_sensor_enabled(true);
//    adc_select_input(4);
//
//    while(true) {
//        vTaskDelay(1000);
//        temperature = read_onboard_temperature();
//        printf("Onboard temperature = %.02f C\n", temperature);
//        xMessageBufferSend( 
//            xControlMessageBuffer,    /* The message buffer to write to. */
//            (void *) &temperature,    /* The source of the data to send. */
//            sizeof( temperature ),    /* The length of the data to send. */
//            0 );                      /* Do not block, should the buffer be full. */
//    }
//}

/* A Task that indefinitely waits for data from temp_task via message buffer. Once received, it will calculate the moving average and prints out the result. */
void movingAverageTask(__unused void *params) 
{
    float fReceivedData;
    float sum = 0;
    size_t xReceivedBytes;
    
    static float data[MAX_TEMP_POINTS] = {0};
    static int index = 0;
    static int count = 0;

    while(true) {
        xReceivedBytes = xMessageBufferReceive( 
            temperatureToMovingTask,        /* The message buffer to receive from. */
            (void *) &fReceivedData,      /* Location to store received data. */
            sizeof( fReceivedData ),      /* Maximum number of bytes to receive. */
            portMAX_DELAY );              /* Wait indefinitely */

            sum -= data[index];            // Subtract the oldest element from sum
            data[index] = fReceivedData;   // Assign the new element to the data
            sum += data[index];            // Add the new element to sum
            index = (index + 1) % MAX_TEMP_POINTS;       // Update the index - make it circular
            
            if (count < MAX_TEMP_POINTS) count++;        // Increment count till it reaches 4

            float movingAverageTemperature = sum / count;
            //printf("Average Temperature = %0.2f C\n", sum / count);
            // SEND ANOTHER MESSAGE of sum/count to the 4th task to print it out
            xMessageBufferSend( 
            movingAverageBufferPrint,    /* The message buffer to write to. */
            (void *) &movingAverageTemperature,    /* The source of the data to send. */
            sizeof( movingAverageTemperature ),    /* The length of the data to send. */
            0 );                      /* Do not block, should the buffer be full. */
    }
}

void simpleAverageTask(__unused void *params) 
{
    float fReceivedData;
    float sum = 0;
    size_t xReceivedBytes;
    static int count = 0;

    while(true) {
        xReceivedBytes = xMessageBufferReceive( 
            temperatureToSimpleTask,        /* The message buffer to receive from. */
            (void *) &fReceivedData,      /* Location to store received data. */
            sizeof( fReceivedData ),      /* Maximum number of bytes to receive. */
            portMAX_DELAY );              /* Wait indefinitely */

            sum += fReceivedData;           // Subtract the oldest element from sum
            //data[index] = fReceivedData;   // Assign the new element to the data
            //sum += data[index];            // Add the new element to sum
            //index = (index + 1) % MAX_TEMP_POINTS;       // Update the index - make it circular
            count++;

            float simpleAverageTemperature = sum / count;
            //if (count < MAX_TEMP_POINTS) count++;        // Increment count till it reaches 4

            //printf("Average Temperature = %0.2f C\n", sum / count);
            // SEND ANOTHER MESSAGE of sum/count to the 4th task to print it out
            xMessageBufferSend( 
            simpleAverageBufferPrint,    /* The message buffer to write to. */
            (void *) &simpleAverageTemperature,    /* The source of the data to send. */
            sizeof( simpleAverageTemperature ),    /* The length of the data to send. */
            0 );                      /* Do not block, should the buffer be full. */
    }
}

void printerTask(__unused void *params)
{
    float movingAverageTemperature;
    float simpleAverageTemperature;
    //float sum = 0;
    size_t xMovingAverageBytes;
    size_t xSimpleAverageBytes;


    while(true) {
        xMovingAverageBytes = xMessageBufferReceive( 
            movingAverageBufferPrint,        /* The message buffer to receive from. */
            (void *) &movingAverageTemperature,      /* Location to store received data. */
            sizeof( movingAverageTemperature ),      /* Maximum number of bytes to receive. */
            portMAX_DELAY );              /* Wait indefinitely */

            printf("The moving Average Temperature is %0.2f C\n", movingAverageTemperature);

        xSimpleAverageBytes = xMessageBufferReceive( 
            simpleAverageBufferPrint,        /* The message buffer to receive from. */
            (void *) &simpleAverageTemperature,      /* Location to store received data. */
            sizeof( simpleAverageTemperature ),      /* Maximum number of bytes to receive. */
            portMAX_DELAY );              /* Wait indefinitely */

            printf("The simple Average Temperature is %0.2f C\n", simpleAverageTemperature);
            //sum += fReceivedData;           // Subtract the oldest element from sum
            //data[index] = fReceivedData;   // Assign the new element to the data
            //sum += data[index];            // Add the new element to sum
            //index = (index + 1) % MAX_TEMP_POINTS;       // Update the index - make it circular
            //count++;

            //float simpleAverageTemperature = sum / count;
            //if (count < MAX_TEMP_POINTS) count++;        // Increment count till it reaches 4

            //printf("Average Temperature = %0.2f C\n", sum / count);
            // SEND ANOTHER MESSAGE of sum/count to the 4th task to print it out
            //xMessageBufferSend( 
            //simpleAverageBufferPrint,    /* The message buffer to write to. */
            //(void *) &simpleAverageTemperature,    /* The source of the data to send. */
            //sizeof( simpleAverageTemperature ),    /* The length of the data to send. */
            //0 );                      /* Do not block, should the buffer be full. */
    }
}

void vLaunch( void) 
{
    TaskHandle_t taskToMeasureTemperature;
    xTaskCreate(main_task, "temperatureReaderThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &taskToMeasureTemperature);

    TaskHandle_t taskToGetMovingAverage;
    xTaskCreate(movingAverageTask, "movingAvgThread", configMINIMAL_STACK_SIZE, NULL, 7, &taskToGetMovingAverage);

    TaskHandle_t taskToGetSimpleAverage;
    xTaskCreate(simpleAverageTask, "simpleAverageThread", configMINIMAL_STACK_SIZE, NULL, 7, &taskToGetSimpleAverage);

    TaskHandle_t taskToPrintOutTemperatures;
    xTaskCreate(printerTask, "printerThread", configMINIMAL_STACK_SIZE, NULL, 5, &taskToPrintOutTemperatures);

    temperatureToMovingTask = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    temperatureToSimpleTask = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    movingAverageBufferPrint = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    simpleAverageBufferPrint = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);


#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
    // takes care of it otherwise)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main( void )
{
    stdio_init_all();

    /* Configure the hardware ready to run the demo. */
    const char *rtos_name;
#if ( portSUPPORT_SMP == 1 )
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
    printf("Starting %s on both cores:\n", rtos_name);
    vLaunch();
#elif ( RUN_FREERTOS_ON_CORE == 1 )
    printf("Starting %s on core 1:\n", rtos_name);
    multicore_launch_core1(vLaunch);
    while (true);
#else
    printf("Starting %s on core 0:\n", rtos_name);
    vLaunch();
#endif
    return 0;
}