#ifndef RANGEFINDER_H
#define RANGEFINDER_H

#include <Arduino.h>

class RangeFinder {

public:
    RangeFinder(int pin_number);
    int get_range_value();
    void set_timer(int timer_number, void (*isr)());

private:
    int pin_number;
    int number_of_reads;
    int stored_vals[7];
    hw_timer_t *Timer_cfg;
    int iterator;
    

    void take_new_readings(int count);
    void clear_stored_vals();
    
};

#endif