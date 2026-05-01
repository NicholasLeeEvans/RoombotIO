#ifndef RANGEFINDER_H
#define RANGEFINDER_H

#include <Arduino.h>

class RangeFinder {

public:
    RangeFinder(int pin_number);
    int get_range_value();
    void set_timer(int timer_number, void (*isr)());
    void take_multiple_readings(int count);

private:
    int pin_number;
    int number_of_reads;
    volatile int stored_vals[7];
    hw_timer_t *Timer_cfg;
    volatile int iterator;
    
    void take_single_reading();
    void clear_stored_vals();
    
};

#endif