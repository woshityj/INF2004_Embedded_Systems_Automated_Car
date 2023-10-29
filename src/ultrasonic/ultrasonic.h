#ifndef ultrasonic
#define ultrasonic

#define TRIGGER_PIN 14
#define ECHO_PIN 15
#define SAMPLING_RATE 30
#define TIMEOUT 26100

void initializeUltrasonic(uint triggerPin, uint echoPin);
void echo_interrupt(uint gpio, uint32_t events);
int64_t alarm_pulldown_callback(alarm_id_t id, void *user_data);
int getMM();

absolute_time_t startTime;
absolute_time_t endTime;
absolute_time_t pulseLength;

#endif