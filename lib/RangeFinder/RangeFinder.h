#ifndef RANGEFINDER_H
#define RANGEFINDER_H

#include <Arduino.h>

class RangeFinder {

public:
    RangeFinder(int pin_number);
    int get_range();

private:
    int pin_number;
    int number_of_reads;
};

#endif