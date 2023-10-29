#ifndef ultrasonic
#define ultrasonic

#define SPEED_OF_SOUND 343 / 1000000 // meters per microsecond
#define DIFFERENCE(a, b) (a<b ? (b-a) : (a-b))
#define TRIGGER_PIN 14
#define ECHO_PIN 15
#define SAMPLING_RATE 30

#endif