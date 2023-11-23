#include "infrared.h"

volatile char barcode_output;

// Helper function to read from an adc input, and check whether the sensor is 
// on a line or not
// 
bool detect_line(int adc_input)
{
    // Variables to calculate the simple average of the IR sensor
    //
    int simpleAvg_index = 0;
    int simpleAvg_sum = 0;
    int simpleAvg_result = 0;

    // Calculate simple average
    //
    while (simpleAvg_index < NUMBER_OF_SAMPLES)
    {
        adc_select_input(adc_input);
        simpleAvg_sum += adc_read();
        simpleAvg_index++;
        
        // Using sleep for now, can change to task delay later on
        //
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    simpleAvg_result = simpleAvg_sum/NUMBER_OF_SAMPLES;

    // printf("%d\n", simpleAvg_result);

    // Line detected
    //
    if (simpleAvg_result > ADC_THRESHOLD)
    {
        return true;
    }
    // Line not detected
    //
    else
    {
        return false;
    }
    
}

bool detect_barcode()
{

    // Stub function for now, will be implemented later
    //

    return true;

}

// Function to read the IR sensor based on the direction the motor vehicle is 
// currently facing and populate the relevant directions ( North, South, East , West )
//
Directions* get_directions(int currentlyFacing)
{
    Directions* dir = (Directions*)malloc(sizeof(Directions));

    if (dir == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // Populate the struct based on the direction the motor vehicle is currently facing
    //
    switch(currentlyFacing)
    {
        // North
        //
        case 1:
            dir->currentlyFacing = currentlyFacing;
            dir->north = detect_line(ADC_FRONT);
            dir->south = NULL;
            dir->east = detect_line(ADC_RIGHT);
            dir->west = detect_line(ADC_LEFT);
            break;
        // South
        //
        case 2:
            dir->currentlyFacing = currentlyFacing;
            dir->north = NULL;
            dir->south = detect_line(ADC_FRONT);
            dir->east = detect_line(ADC_LEFT);
            dir->west = detect_line(ADC_RIGHT);
            break;
        // East
        //
        case 3:
            dir->currentlyFacing = currentlyFacing;
            dir->north = detect_line(ADC_LEFT);
            dir->south = detect_line(ADC_RIGHT);
            dir->east = detect_line(ADC_FRONT);
            dir->west = NULL;
            break;
            break;
        // West
        //
        case 4:
            dir->currentlyFacing = currentlyFacing;
            dir->north = detect_line(ADC_RIGHT);
            dir->south = detect_line(ADC_LEFT);
            dir->east = NULL;
            dir->west = detect_line(ADC_FRONT);
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

// Callback function used by the repeating timer to scan the barcode
//
char IR_barcode_scan()
{
    // Static variables to store the moving average of the IR sensor
    //
    static int movingAvg_data[NUMBER_OF_SAMPLES] = {0};
    static int movingAvg_index = 0;
    static int movingAvg_count = 0;
    static int movingAvg_sum = 0;
    static int movingAvg_result = 0;

    // Static variables to keep track of the state of the barcode scanner
    //
    static bool flag = true;
    static bool validBarcode = false;
    static bool isBackwards = false;
    static bool charDetected = false;
    static int currentIndex = 0;

    // Static variables to store the decoded character and the reverse decoded character
    //
    static char decoded_character;
    static char decoded_character_reverse;

    // Static variable to store the character that is detected
    //
    static char barcode_char;

    // Static arrays to store the timings, timing differences and binary array
    //
    static uint64_t timings[TIMING_BUFFERSIZE];
    static uint64_t timing_differences[TIMINGDIFFERENCES_BUFFERSIZE];
    static int char_binary_array[TIMINGDIFFERENCES_BUFFERSIZE];

    // Select IR sensor to read from
    //
    adc_select_input(ADC_FRONT);

    // Calculate moving average
    //
    movingAvg_sum -= movingAvg_data[movingAvg_index];
    movingAvg_data[movingAvg_index] = adc_read();
    movingAvg_sum += movingAvg_data[movingAvg_index];
    movingAvg_index = (movingAvg_index + 1) % NUMBER_OF_SAMPLES;

    if (movingAvg_count < NUMBER_OF_SAMPLES) movingAvg_count++;

    movingAvg_result = movingAvg_sum/movingAvg_count;

    // Check if bar is black
    //
    if (movingAvg_result > ADC_THRESHOLD)
    {

        // If flag is true, then the previous bar was white, so store the time
        //
        if (flag)
        {

            // Set flag to false to prevent time storing for the same bar
            //
            flag = false;
            timings[currentIndex] = time_us_64();
            currentIndex+=1;
        }
    }
    // Else bar is white
    //
    else
    {
        // If flag is false, then the previous bar was black, so store the time
        //
        if (!flag)
        {
            flag = true;
            timings[currentIndex] = time_us_64();
            currentIndex += 1;
        }
    }

    // Buffer is full, try to decode the buffer
    //
    if (currentIndex == TIMING_BUFFERSIZE)
    {

        // Variables to store the top 3 timings
        //
        uint64_t first, second, third;

        // Call helper function for data manipulation in arrays
        //
        calculate_timing_difference(timings, timing_differences);
        find_top_three_timings(timing_differences, &first, &second, &third);
        form_binary_array(timing_differences, char_binary_array, first, second, third);
        print_array(char_binary_array);
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

            // Check if the barcode starts with *, which means valid
            //
            if (decoded_character == '*')
            {
                validBarcode = true;
                printf("Valid barcode detected\n");
            }
            // Check if the barcode is being scanned backwards and starts with *, which means valid
            //
            else if (decoded_character_reverse == '*')
            {
                validBarcode = true;
                isBackwards = true;
                printf("Valid barcode detected\n");
            }
            // Invalid barcode detected
            //
            else
            {
                printf("Error reading barcode OR Invalid barcode detected\n");
            }
        }
        // Read the character
        //
        else if (!charDetected)
        {
            // Set flag to true to identify that barcode character has been detected
            //
            charDetected = true;

            // Check if barcode is being scanned backwards
            //
            if (isBackwards)
            {
                // Reverse the binary array and decode it
                //
                reverse_array(char_binary_array);
                decoded_character_reverse = decode_array(char_binary_array);
                barcode_char = decoded_character_reverse;
            }
            // Barcode is being scanned normally
            //
            else
            {
                // Decode the binary array
                //
                decoded_character = decode_array(char_binary_array);
                barcode_char = decoded_character;
            }
        }
        // Detect end of barcode to check for valid barcode and reset all flags and variables
        //
        else
        {   
            // Check if barcode is being scanned backwards
            //
            if (isBackwards)
            {
                // Reverse the binary array and decode it
                //
                reverse_array(char_binary_array);
                decoded_character_reverse = decode_array(char_binary_array);
                
                // Check for * to identify valid barcode, if invalid print error message
                //
                if (decoded_character_reverse != '*')
                {
                    printf("Error reading barcode OR Invalid barcode detected\n");
                }
                // Valid barcode, print the character
                //
                else
                {
                    printf("Character = %c\n", barcode_char);
                    return barcode_char;
                }
            }
            // Barcode is being scanned normally
            //
            else
            {
                // Decode the binary array
                //
                decoded_character = decode_array(char_binary_array);

                // Check for * to identify valid barcode, if invalid print error message
                //
                if (decoded_character != '*')
                {
                    printf("Error reading barcode OR Invalid barcode detected\n");
                }
                // Valid barcode, print the character
                //
                else
                {
                    printf("Character = %c\n", barcode_char);
                    return barcode_char;
                }
            }

            // Reset all variables
            //
            flag = true;
            validBarcode = false;
            isBackwards = false;
            charDetected = false;
        }
        // Reset the index
        //
        currentIndex = 0;
    }
    
    return '?';
    
}

// Helper function to calculate the timing difference between 2 timings based on a given array 
// and use the results to populate another array
//
void calculate_timing_difference(uint64_t timings[], uint64_t timing_differences[])
{
    for (int i = 0; i < TIMINGDIFFERENCES_BUFFERSIZE; i++)
    {
        timing_differences[i] = timings[i+1] - timings[i];
    }
}

// Helper function to find the top 3 timings in an array
//
void find_top_three_timings(uint64_t arr[], uint64_t *first, uint64_t *second, uint64_t *third)
{
    *first = 0;
    *second = 0;
    *third = 0;

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

// Helper function to convert the timing_differences array to a binary array
//
void form_binary_array(uint64_t timing_differences[], int char_binary_array[], uint64_t first, uint64_t second, uint64_t third)
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

// Helper function to print an array
//
void print_array(int arr[])
{
    for (int i = 0; i < BINARYARRAY_BUFFERSIZE; i++)
    {
        printf("%d\n", arr[i]);
    }
    printf("\n");

}

// Helper function to decode a binary array into it's relevant character using code39 format
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

// Helper function to reverse a given array
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

// Function to initialize ADC functions, and all GPIO pins for adc functions
//
void IR_init()
{
    adc_init();
    adc_gpio_init(IR_PIN_FRONT);
    adc_gpio_init(IR_PIN_LEFT);
    adc_gpio_init(IR_PIN_RIGHT);

    // add_repeating_timer_ms(SAMPLE_RATE_MS, IR_barcode_scan, NULL, &timer);
}