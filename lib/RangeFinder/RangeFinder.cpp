#include "RangeFinder.h"

RangeFinder::RangeFinder(int pin_number){
    this->pin_number = pin_number;
    this->number_of_reads = 5;
    pinMode(pin_number, INPUT);
    this->iterator = 0;
    clear_stored_vals();
    
}

void RangeFinder::set_timer(int timer_number, void (*isr)()){
    Timer_cfg = timerBegin(timer_number, 80, true); 
    timerAttachInterrupt(Timer_cfg, isr, true);
    //timerAlarmWrite(Timer_cfg, 500, true);
    timerAlarmWrite(Timer_cfg, 2000, true); //for variable interrupt timing
    timerAlarmEnable(Timer_cfg);
}

void RangeFinder::clear_stored_vals(){
    for(int itr = 0; itr < 7; itr++){
        this->stored_vals[itr] = 0;
    }
}

void RangeFinder::take_new_readings(int count){
    if(count < 1){
        count = 1;
    }else if(count > 7){
        count = 7;
    }
        
    for(int itr = 0; itr < count; itr++){
        analogRead(this->pin_number);
        this->stored_vals[iterator] = analogRead(this->pin_number);
        iterator = (iterator + 1) % 7;
    }

}

int RangeFinder::get_range_value(){

    take_new_readings(7);

    //sort the stored values, (probably dont need to swap them all just find the min and max them subtract from total...)
    bool swapped;
    do{
        swapped = false;
        for(int itr = 1; itr < 7; itr++){
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
    for(int itr = 1; itr < 6; itr++){
        sum += stored_vals[itr];
    }

    return sum / 5;
    
}
