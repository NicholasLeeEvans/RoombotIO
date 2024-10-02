#include "RangeFinder.h"

RangeFinder::RangeFinder(int pin_number){
    this->pin_number = pin_number;
    this->number_of_reads = 5;
    pinMode(pin_number, INPUT);
}

int RangeFinder::get_range(){
    int sum = 0;
    for(int itr = 0; itr < this->number_of_reads; itr++){
        sum += analogRead(this->pin_number);
        delay(5);
    }
    return sum / this->number_of_reads;
}