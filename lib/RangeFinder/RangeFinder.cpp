#include "RangeFinder.h"

#define ISR_TIMER_DIVIDER   80

#define STORED_READS    7

RangeFinder::RangeFinder(int pin_number){
    this->pin_number = pin_number;
    pinMode(pin_number, INPUT);
    this->iterator = 0;
    clear_stored_vals();
    
}

void RangeFinder::set_timer(int timer_number, void (*isr)()){
    Timer_cfg = timerBegin(timer_number, ISR_TIMER_DIVIDER, true); 
    timerAttachInterrupt(Timer_cfg, isr, true);
    //timerAlarmWrite(Timer_cfg, 500, true);
    timerAlarmWrite(Timer_cfg, 300 * 1000, true); //for variable interrupt timing
    timerAlarmEnable(Timer_cfg);
}

void RangeFinder::clear_stored_vals(){
    for(int itr = 0; itr < STORED_READS; itr++){
        this->stored_vals[itr] = 0;
    }
}

void RangeFinder::take_single_reading(){
    analogRead(this->pin_number);
    this->stored_vals[iterator] = analogRead(this->pin_number);
    iterator = (iterator + 1) % STORED_READS;
}

void RangeFinder::take_multiple_readings(int count){
    if(count < 1){
        count = 1;
    }else if(count > STORED_READS){
        count = STORED_READS;
    }
        
    for(int itr = 0; itr < count; itr++){
        this->take_single_reading();
    }

}

int RangeFinder::get_range_value(){

    //this->take_multiple_readings(7);

    //sort the stored values, (probably dont need to swap them all just find the min and max them subtract from total...)
    bool swapped;
    do{
        swapped = false;
        for(int itr = 1; itr < STORED_READS; itr++){
            if(stored_vals[itr] < stored_vals[itr - 1]){
                int temp_value = stored_vals[itr];
                stored_vals[itr] = stored_vals[itr - 1];
                stored_vals[itr - 1] = temp_value;
                swapped = true;
            }
        }
    } while(swapped == true);
    //remove the top and bottom, then average the remaining...
    int sum = 0;
    for(int itr = 1; itr < STORED_READS - 1; itr++){
        sum += stored_vals[itr];
    }

    return sum / (STORED_READS - 2);
    
}
