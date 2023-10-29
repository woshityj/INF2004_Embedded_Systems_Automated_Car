#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>

#include "hardware/adc.h"
#include "infrared.h"
#include "../mapping/mapper.h"

#include "pico/stdlib.h"

bool detect_wall()
{
    // Line detected
    //
    if (adc_read() > 1500)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

Directions* get_directions(int currentlyFacing)
{
    Directions* dir = (Directions*)malloc(sizeof(Directions));

    if (dir == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    switch(currentlyFacing)
    {
        // North
        //
        case 1:
            dir->currentlyFacing = currentlyFacing;
            adc_select_input(ADC_FRONT);
            dir->north = detect_wall();
            dir->south = NULL;
            adc_select_input(ADC_RIGHT);
            dir->east = detect_wall();
            adc_select_input(ADC_LEFT);
            dir->west = adc_read();
            break;
        // South
        //
        case 2:
            dir->currentlyFacing = currentlyFacing;
            dir->north = NULL;
            adc_select_input(ADC_FRONT);
            dir->south = detect_wall();
            adc_select_input(ADC_RIGHT);
            dir->east = detect_wall();
            adc_select_input(ADC_LEFT);
            dir->west = adc_read();
            break;
        // East
        //
        case 3:
            dir->currentlyFacing = currentlyFacing;
            adc_select_input(ADC_LEFT);
            dir->north = detect_wall();
            adc_select_input(ADC_RIGHT);
            dir->south = detect_wall();
            adc_select_input(ADC_FRONT);
            dir->east = adc_read();
            dir->west = NULL;
            break;
        // West
        //
        case 4:
            dir->currentlyFacing = currentlyFacing;
            adc_select_input(ADC_RIGHT);
            dir->north = detect_wall();
            adc_select_input(ADC_LEFT);
            dir->south = detect_wall();
            dir->east = NULL;
            adc_select_input(ADC_FRONT);
            dir->west = adc_read();
            break;

        default:
            printf("Invalid direction code\n");
            // Free memory
            //
            free(dir);
            return NULL;
    }

    // Return the pointer to the new struct
    //
    return dir;
}

// Callback for repeating timer to scan barcode
//
bool IR_barcode_scan(struct repeating_timer *t)
{
    static bool flag = true;
    static bool validBarcode = false;
    static bool isBackwards = false;
    static bool charDetected = false;
    static int currentIndex = 0;

    static char decoded_character;
    static char decoded_character_reverse;

    static char barcode_char;

    static float timings[TIMING_BUFFERSIZE];
    static float timing_differences[TIMINGDIFFERENCES_BUFFERSIZE];
    static int char_binary_array[TIMINGDIFFERENCES_BUFFERSIZE];

    adc_select_input(0);
    
    // Check if bar is black
    //
    if (adc_read() > 1500)
    {
        if (flag)
        {
            flag = false;
            timings[currentIndex] = to_us_since_boot(get_absolute_time());
            currentIndex+=1;
        }
    }
    // Else bar is white
    //
    else
    {
        if (!flag)
        {
            flag = true;
            timings[currentIndex] = to_us_since_boot(get_absolute_time());
            currentIndex += 1;
        }
    }

    // Buffer is full, try to decode the buffer
    //
    if (currentIndex == TIMING_BUFFERSIZE)
    {
        float first, second, third;

        calculate_timing_difference(timings, timing_differences);
        find_top_three_timings(timing_differences, &first, &second, &third);
        form_binary_array(timing_differences, char_binary_array, first, second, third);

        // Check for valid barcode
        //
        if (!validBarcode)
        {   
            // Decode the binary array
            //
            decoded_character = decode_array(char_binary_array);

            // Decode the reverse binary array
            //
            reverse_array(char_binary_array);
            decoded_character_reverse = decode_array(char_binary_array);

            if (decoded_character == '*')
            {
                validBarcode = true;
                printf("Valid barcode detected\n");
            }
            else if (decoded_character_reverse == '*')
            {
                validBarcode = true;
                isBackwards = true;
                printf("Valid barcode detected\n");
            }
            else
            {
                printf("Error reading barcode OR Invalid barcode detected\n");
            }
        }
        // Read the character
        //
        else if (!charDetected)
        {
            charDetected = true;

            if (isBackwards)
            {
                reverse_array(char_binary_array);
                decoded_character_reverse = decode_array(char_binary_array);
                barcode_char = decoded_character_reverse;
            }
            else
            {
                decoded_character = decode_array(char_binary_array);
                barcode_char = decoded_character;
            }
        }
        // Detect end of barcode to check for valid barcode and reset all flags and variables
        //
        else
        {   
            if (isBackwards)
            {
                reverse_array(char_binary_array);
                decoded_character_reverse = decode_array(char_binary_array);
                
                if (decoded_character_reverse != '*')
                {
                    printf("Error reading barcode OR Invalid barcode detected\n");
                }
                else
                {
                    printf("%c", barcode_char);
                }
            }
            else
            {
                decoded_character = decode_array(char_binary_array);

                if (decoded_character != '*')
                {
                    printf("Error reading barcode OR Invalid barcode detected\n");
                }
                else
                {
                    printf("Character = %c\n", barcode_char);
                }
            }

            // Reset all variables
            //
            flag = true;
            validBarcode = false;
            isBackwards = false;
            charDetected = false;
        }

        currentIndex = 0;
    }
    
    return true;
    
}

// Function to calculate the timing differences between 2 state change points and store them into an array
//
void calculate_timing_difference(float timings[], float timing_differences[])
{
    for (int i = 0; i < TIMINGDIFFERENCES_BUFFERSIZE; i++)
    {
        double first_timing_s = timings[i]/1000000.0;
        double second_timing_s = timings[i+1]/1000000.0;
        timing_differences[i] = second_timing_s - first_timing_s;
    }
}

// Function to find the top 3 timings of an array
//
void find_top_three_timings(float arr[], float *first, float *second, float *third)
{
    *first = FLT_MIN;
    *second = FLT_MIN;
    *third = FLT_MIN;

    for (int i = 0; i < TIMINGDIFFERENCES_BUFFERSIZE; i++)
    {
        if (arr[i] > *first)
        {
            *third = *second;
            *second = *first;
            *first = arr[i];
        } else if (arr[i] > *second)
        {
            *third = *second;
            *second = arr[i];
        } else if (arr[i] > *third)
        {
            *third = arr[i];
        }
    }
}

// Function to form transform form a binary array based on the timing difference array
//
void form_binary_array(float timing_differences[], int char_binary_array[], float first, float second, float third)
{
    for (int i = 0; i < BINARYARRAY_BUFFERSIZE; i++) {
        if (timing_differences[i] == first || timing_differences[i] == second || timing_differences[i] == third)
        {
            char_binary_array[i] = 1;
        } else
        {
            char_binary_array[i] = 0;
        }
    }
}

void print_array(int arr[])
{
    for (int i = 0; i < BINARYARRAY_BUFFERSIZE; i++)
    {
        printf("%d", arr[i]);
    }
    printf("\n");

}

// Functoion to decode binary array into a character
//
char decode_array(int char_binary_array[])
{
    Code39Mapping code39Mappings[] =
    {
        {{0, 1, 0, 0, 1, 0, 1, 0, 0}, '*'},
        {{1, 0, 0, 0, 0, 1, 0, 0, 1}, 'A'},
        {{0, 0, 1, 0, 0, 1, 0, 0, 1}, 'B'},
        {{1, 0, 1, 0, 0, 1, 0, 0, 0}, 'C'},
        {{0, 0, 0, 0, 1, 1, 0, 0, 1}, 'D'},
        {{1, 0, 0, 0, 1, 1, 0, 0, 0}, 'E'},
        {{0, 0, 1, 0, 1, 1, 0, 0, 0}, 'F'},
        {{0, 0, 0, 0, 0, 1, 1, 0, 1}, 'G'},
        {{1, 0, 0, 0, 0, 1, 1, 0, 0}, 'H'},
        {{0, 0, 1, 0, 0, 1, 1, 0, 0}, 'I'},
        {{0, 0, 0, 0, 1, 1, 1, 0, 0}, 'J'},
        {{1, 0, 0, 0, 0, 0, 0, 1, 1}, 'K'},
        {{0, 0, 1, 0, 0, 0, 0, 1, 1}, 'L'},
        {{1, 0, 1, 0, 0, 0, 0, 1, 0}, 'M'},
        {{0, 0, 0, 0, 1, 0, 0, 1, 1}, 'N'},
        {{1, 0, 0, 0, 1, 0, 0, 1, 0}, 'O'},
        {{0, 0, 1, 0, 1, 0, 0, 1, 0}, 'P'},
        {{0, 0, 0, 0, 0, 0, 1, 1, 1}, 'Q'},
        {{1, 0, 0, 0, 0, 0, 1, 1, 0}, 'R'},
        {{0, 0, 1, 0, 0, 0, 1, 1, 0}, 'S'},
        {{0, 0, 0, 0, 1, 0, 1, 1, 0}, 'T'},
        {{1, 1, 0, 0, 0, 0, 0, 0, 1}, 'U'},
        {{0, 1, 1, 0, 0, 0, 0, 0, 1}, 'V'},
        {{1, 1, 1, 0, 0, 0, 0, 0, 0}, 'W'},
        {{0, 1, 0, 0, 1, 0, 0, 0, 1}, 'X'},
        {{1, 1, 0, 0, 1, 0, 0, 0, 0}, 'Y'},
        {{0, 1, 1, 0, 1, 0, 0, 0, 0}, 'Z'},
        {{0, 1, 0, 0, 0, 0, 1, 0, 1}, '-'},
        {{1, 1, 0, 0, 0, 0, 1, 0, 0}, '.'},
        {{0, 1, 0, 1, 0, 1, 0, 0, 0}, '$'},
        {{0, 1, 0, 1, 0, 0, 0, 1, 0}, '/'},
        {{0, 1, 0, 0, 0, 1, 0, 1, 0}, '+'},
        {{0, 0, 0, 1, 0, 1, 0, 1, 0}, '%'},
        {{0, 0, 0, 1, 1, 0, 1, 0, 0}, '0'},
        {{1, 0, 0, 1, 0, 0, 0, 0, 1}, '1'},
        {{0, 0, 1, 1, 0, 0, 0, 0, 1}, '2'},
        {{1, 0, 1, 1, 0, 0, 0, 0, 0}, '3'},
        {{0, 0, 0, 1, 1, 0, 0, 0, 1}, '4'},
        {{1, 0, 0, 1, 1, 0, 0, 0, 0}, '5'},
        {{0, 0, 1, 1, 1, 0, 0, 0, 0}, '6'},
        {{0, 0, 0, 1, 0, 0, 1, 0, 1}, '7'},
        {{1, 0, 0, 1, 0, 0, 1, 0, 0}, '8'},
        {{0, 0, 1, 1, 0, 0, 1, 0, 0}, '9'},
    };

    for (int i = 0; i < CODE39BUFFER; ++i)
    {
        int matches = 1;
        Code39Mapping mapping = code39Mappings[i];
        for (int j = 0; j < BINARYARRAY_BUFFERSIZE; ++j)
        {
            if (char_binary_array[j] != mapping.binary[j])
            {
                matches = 0;
                break;
            }
        }
        if (matches)
        {
            return mapping.character;
        }
    }

    return '?';
}

// Function to reverse the contents of an array
//
void reverse_array(int arr[])
{
    int start = 0;
    int end = BINARYARRAY_BUFFERSIZE - 1;

    while (start < end)
    {
        int temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

// Function to intialise the IR sensor
//
void IR_init()
{
    adc_init();
    adc_gpio_init(IR_PIN_LEFT);
}

// Main function
//
int main()
{
    stdio_init_all();
    IR_init();

    add_repeating_timer_ms(SAMPLE_RATE_MS, IR_barcode_scan, NULL, &timer);

    while (1)
    {

    };

    return 0;
}