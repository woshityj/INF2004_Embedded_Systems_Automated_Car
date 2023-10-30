#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "ultrasonic.h"


#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )
struct repeating_timer timer;
// Abstracted task returns
// to be done

int getMM()
{
    pulseLength = absolute_time_diff_us(startTime, endTime); // get the echo pin return wave form length
    int milliimeters = pulseLength / 6;
    if(milliimeters < 20 || milliimeters > 4000 || pulseLength >= TIMEOUT) // range of the HC-SR04P
    {
        milliimeters = 0; // if out of range then just assume that it was a 0
    }
    // reset the parameters for the next echo_interrupt
    endTime = 0;
    startTime = 0;
    return milliimeters;
}

void initializeUltrasonic(uint triggerPin, uint echoPin)
{
    // pin initializings
    gpio_init(triggerPin);
    gpio_init(echoPin);

    gpio_set_dir(triggerPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
}

int64_t alarm_pulldown_callback(alarm_id_t id, void *user_data)
{
    gpio_put(TRIGGER_PIN, 0); // Pull the trigger pin down to signify the sonic burst start   
    return 0;
}

void echo_interrupt(uint gpio, uint32_t events)
{
    if(events & GPIO_IRQ_EDGE_RISE) // Ultrasonic sends the signal back
    {
        if(startTime == 0)
        {
            startTime = get_absolute_time();    
        }
    }

    else if(events & GPIO_IRQ_EDGE_FALL) // Ultrasonic finishes sending signal
    {
        if(endTime == 0)
        {
            endTime = get_absolute_time();
        }
    }
}

bool pullTrigger(struct repeating_timer *t)
{
    gpio_put(TRIGGER_PIN, 1);

     // requires AT LEAST 10us for the sonic burst
    add_alarm_in_us(15, &alarm_pulldown_callback, NULL, false);

    return true;
}

void main_task(__unused void *params)
{
    printf("initializing the sensors\n");

    initializeUltrasonic(TRIGGER_PIN, ECHO_PIN);

    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_interrupt);
    add_repeating_timer_ms(-SAMPLING_RATE, pullTrigger, NULL, &timer);

    while(true)
    {
        if(endTime > startTime)
        {
            int millimeters = getMM();
            printf("The distance is: %dmm\n", millimeters);
        }
        vTaskDelay(1);
    }
}
void vLaunch(void)
{

    TaskHandle_t ultrasonicTask;
    xTaskCreate(main_task, "ultrasonic Sensor", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &ultrasonicTask);
    vTaskCoreAffinitySet(ultrasonicTask, 0x3);

    vTaskStartScheduler();
}

int main(void)
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