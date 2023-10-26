#define IR_PIN_FRONT 26 // ADC 0 
#define IR_PIN_LEFT 27 // ADC 1
#define IR_PIN_RIGHT 28 // ADC 2

#define ADC_FRONT 0
#define ADC_LEFT 1
#define ADC_RIGHT 2

#define SAMPLE_RATE_MS 10
#define TIMING_BUFFERSIZE 10
#define TIMINGDIFFERENCES_BUFFERSIZE 9
#define BINARYARRAY_BUFFERSIZE 9
#define CODE39BUFFER 43

struct repeating_timer timer;

typedef struct {
    int currentlyFacing;
    bool north;
    bool south;
    bool east;
    bool west;
} Directions;

typedef struct {
    int binary[9];
    char character;
} Code39Mapping;

void IR_init();
bool detect_wall();
Directions* get_directions(int currentlyFacing);
bool IR_barcode_scan(struct repeating_timer *t);
void calculate_timing_difference(float timings[], float timing_differences[]);
void find_top_three_timings(float arr[], float *first, float *second, float *third);
void form_binary_array(float timing_differences[], int char_binary_array[], float first, float second, float third);
char decode_array(int char_binary_array[]);
void reverse_array(int arr[]);