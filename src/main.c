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

#include "pathfinder/algorithms.h"
#define CORE_ONE 0X2
#define CORE_ZERO 0x1
#define BOTH_CORES 0x3

#define mbaTASK_MESSAGE_BUFFER_SIZE       ( 60 )
#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif
#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

// Change this to however many temperature points for the movingAverage calcs
#define MAX_TEMP_POINTS 10

/* Global Variables initialization */
static MessageBufferHandle_t temperatureToMovingTask;
static MessageBufferHandle_t temperatureToSimpleTask;
static MessageBufferHandle_t movingAverageBufferPrint;
static MessageBufferHandle_t simpleAverageBufferPrint;
bool connected = 0;
/* End of Global Variables initialization */

float read_onboard_temperature() {
    // Reads and converts the voltage to temperature in degrees C and returns it
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

        xMessageBufferSend( 
            temperatureToMovingTask,  /* Write to the movingTask buffer */
            (void *) &temperature,    /* Get the data to put into the buffer from the temperature variable */
            sizeof( temperature ),    /* declare the sizeof() the temperature variable for dynamic reasons */
            0 );                      /* Do not block, should the buffer be full. */
        
        xMessageBufferSend( 
            temperatureToSimpleTask,  /* Write to the simpleTask buffer */
            (void *) &temperature,    /* repeat comment of the above, just to a different buffer */
            sizeof( temperature ),    
            0 );                      
    }
}

void connectWifi(__unused void *params)
{
    while(!connected)
    {

        if (cyw43_arch_init()) {
            printf("failed to initialise\n");
            return;
        }
        cyw43_arch_enable_sta_mode();
        printf("Connecting to Wi-Fi...\n");
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
            connected = 0;
            printf("failed to connect.\n");
        } else {
            connected = 1;
            printf("Connected.\n");
        }
        
        while(connected)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        };
        
        cyw43_arch_deinit();
    }
}
/* A Task that indefinitely waits for data from main_task via message buffer. 
Once received, it will calculate the moving average and prints out the result. */

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
            temperatureToMovingTask,                        /* Receive the message from the simpleTask buffer from main_task */
            (void *) &fReceivedData,                        /* Location to store received data. */
            sizeof( fReceivedData ),                        /* Maximum number of bytes to receive. */
            portMAX_DELAY );                                /* Wait indefinitely */

            sum -= data[index];                             /* Subtract the oldest element from sum */
            data[index] = fReceivedData;                    /* Assign the new element to the data */
            sum += data[index];                             /* Add the new element to sum */
            index = (index + 1) % MAX_TEMP_POINTS;          /* Update the index - make it circular */
            
            if (count < MAX_TEMP_POINTS) count++;           /* Increment count till it reaches MAX_TEMP_POINTS */

            float movingAverageTemperature = sum / count;   /* Calculate the movingAverageTemperature */

            xMessageBufferSend( 
            movingAverageBufferPrint,                       /* Write to the movingPrinting buffer */
            (void *) &movingAverageTemperature,    
            sizeof( movingAverageTemperature ),    
            0 );
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
            temperatureToSimpleTask,                        /* Receive the message from the simpleTask buffer from main_task */
            (void *) &fReceivedData,                        /* Same as the movingAverageTask description */
            sizeof( fReceivedData ),      
            portMAX_DELAY );              

            sum += fReceivedData;
            count++;

            float simpleAverageTemperature = sum / count;

            xMessageBufferSend( 
            simpleAverageBufferPrint,                       /* Write to the simplePrinting buffer */
            (void *) &simpleAverageTemperature,    
            sizeof( simpleAverageTemperature ),    
            0 );                      
    }
}


/* The task that contains all of the printf() statements as stipulated in the lab requirements */
void printerTask(__unused void *params)
{
    float movingAverageTemperature;
    float simpleAverageTemperature;
    size_t xMovingAverageBytes;
    size_t xSimpleAverageBytes;

    /* Both these buffer receivers just receive the temperature value to print from their respective functions */
    while(true) {
        xMovingAverageBytes = xMessageBufferReceive( 
            movingAverageBufferPrint,       
            (void *) &movingAverageTemperature,     
            sizeof( movingAverageTemperature ),      
            portMAX_DELAY );            

        xSimpleAverageBytes = xMessageBufferReceive( 
            simpleAverageBufferPrint,      
            (void *) &simpleAverageTemperature,      
            sizeof( simpleAverageTemperature ),    
            portMAX_DELAY );         
            
            float currentTemperature = read_onboard_temperature();

            printf("This is the printer task!\n");
            printf("The current temperature is %0.2f C\n", currentTemperature);
            printf("The moving Average Temperature is %0.2f C\n", movingAverageTemperature);
            printf("The simple Average Temperature is %0.2f C\n", simpleAverageTemperature);
            printFromAlgo();
            
    }
}

void vLaunch( void) 
{
    /* Main task launch handler and initializers */

    TaskHandle_t taskToMeasureTemperature;
    xTaskCreate(main_task, "temperatureReaderThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &taskToMeasureTemperature);
    vTaskCoreAffinitySet(taskToMeasureTemperature, BOTH_CORES);

    TaskHandle_t taskToGetMovingAverage;
    xTaskCreate(movingAverageTask, "movingAvgThread", configMINIMAL_STACK_SIZE, NULL, 7, & taskToGetMovingAverage);
    vTaskCoreAffinitySet(taskToGetMovingAverage, CORE_ZERO);

    TaskHandle_t taskToGetSimpleAverage;
    xTaskCreate(simpleAverageTask, "simpleAverageThread", configMINIMAL_STACK_SIZE, NULL, 7, &taskToGetSimpleAverage);
    vTaskCoreAffinitySet(taskToGetSimpleAverage, CORE_ZERO);

    TaskHandle_t taskToPrintOutTemperatures;
    xTaskCreate(printerTask, "printerThread", configMINIMAL_STACK_SIZE, NULL, 5, &taskToPrintOutTemperatures);
    vTaskCoreAffinitySet(taskToPrintOutTemperatures, CORE_ONE);

    TaskHandle_t wifiTask;
    xTaskCreate(connectWifi, "wifi Connection", configMINIMAL_STACK_SIZE, NULL, 1, &wifiTask);
    vTaskCoreAffinitySet(wifiTask, CORE_ONE);

    temperatureToMovingTask = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    temperatureToSimpleTask = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    movingAverageBufferPrint = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    simpleAverageBufferPrint = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);

    UBaseType_t taskOne = vTaskCoreAffinityGet(taskToMeasureTemperature);
    printf("Task One is on Core(s): %d\n", taskOne-1);

    UBaseType_t taskTwo = vTaskCoreAffinityGet(taskToGetMovingAverage);
    printf("Task Two is on Core: %d\n", taskTwo-1);
    
    UBaseType_t taskThree = vTaskCoreAffinityGet(taskToGetSimpleAverage);
    printf("Task Three is on Core: %d\n", taskThree-1);

    UBaseType_t taskFour = vTaskCoreAffinityGet(taskToPrintOutTemperatures);
    printf("Task Four is on Core: %d\n", taskFour-1);

    UBaseType_t taskFive = vTaskCoreAffinityGet(wifiTask);
    printf("Task Five is on Core: %d\n", taskFive-1);

//#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
//    vTaskCoreAffinitySet(main_task, 1);
//#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main( void )
{
    // Initialization
    stdio_init_all();

    const char *rtos_name;
#if ( portSUPPORT_SMP == 1 )
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
    sleep_ms(5000);
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