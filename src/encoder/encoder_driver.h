
void gpio_callback_isr(uint gpio, uint32_t events);
bool repeating_timer_callback_isr(struct repeating_timer *t);
int get_wheel_interrupt_speed(uint gpio);
float get_wheel_speed(uint gpio);
void encoder_driver_init();