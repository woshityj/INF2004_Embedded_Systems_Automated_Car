#ifndef infrared
#define infrared

#define IR_PIN 15
#define MAX_TIMINGS 10
#define MAX_TIMINGDIFFERENCES 9

void IR_scan();
void IR_init();
void formChar(float first, float second, float third);
void findTopThree(float arr[], float *first, float *second, float *third);

#endif