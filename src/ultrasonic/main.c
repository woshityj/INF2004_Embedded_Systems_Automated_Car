#include "ultrasonic.h"
#include "ultrasonic.c"


// TEST CASE

int main(void)
{
    stdio_init_all();

    // Sleep for time to turn on serial monitor
    sleep_ms(3000);
    printf("initializing the sensors\n");

    initializeUltrasonic(TRIGGER_PIN, ECHO_PIN);

    // Simulating the FreeRTOS-Kernel task while loop
    while(true)
    {
        bool result = getObstacle();
        printf("[Test] %d", result);
        // Filtering out the "too high" and "too low" values
        // if(millimeters > 0)
        // {
        //     // Integration code adds to a moving buffer to get a more smooth reading
        //     printf("The distance is: %dmm\n", millimeters);
        // }

        // This sleep simulates the vTaskDelay() of 50ms, this simulates it being a non-blocking delay when using FreeRTOS-Kernel
        sleep_ms(50);
        // This value cannot be lower than the SAMPLING_RATE value of the repeating timer, as it needs time to complete it's critical section
    }
}