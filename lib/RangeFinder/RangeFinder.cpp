#include "RangeFinder.h"

#define ISR_TIMER_DIVIDER       80
#define SCAN_TIMER_INTERVAL_US  (40 * 1000)

#define STORED_READS    7
#define MEDIAN_ITR      3

RangeFinder::RangeFinder(int pin_number){
    this->pin_number = pin_number;
    pinMode(pin_number, INPUT);
    this->iterator = 0;
    clear_stored_vals();
    
}

void RangeFinder::set_timer(int timer_number, void (*isr)()){
    Timer_cfg = timerBegin(timer_number, ISR_TIMER_DIVIDER, true); 
    timerAttachInterrupt(Timer_cfg, isr, true);
    timerAlarmWrite(Timer_cfg, SCAN_TIMER_INTERVAL_US, true);
    timerAlarmEnable(Timer_cfg);
}

void RangeFinder::clear_stored_vals(){
    for(int itr = 0; itr < STORED_READS; itr++){
        this->stored_vals[itr] = 0;
    }
}

void RangeFinder::sample(){
    analogRead(this->pin_number);
    this->stored_vals[iterator] = analogRead(this->pin_number);
    iterator = (iterator + 1) % STORED_READS;
}

int RangeFinder::get_range_value(){

    int read_values[STORED_READS];
    noInterrupts();
    for(int i = 0; i < STORED_READS; i++) {
        read_values[i] = stored_vals[i];
    }
    interrupts();
    for(int itr = 1; itr < STORED_READS; itr++)
    {
        for(int itr2 = itr; itr2 > 0; itr2--){
            int check_value = read_values[itr2];
            int comp_value = read_values[itr2 - 1];
            if(comp_value < check_value){
                break;
            }
            read_values[itr2] = comp_value;
            read_values[itr2 - 1] = check_value;
        }
    }
    return read_values[MEDIAN_ITR];

    
}
