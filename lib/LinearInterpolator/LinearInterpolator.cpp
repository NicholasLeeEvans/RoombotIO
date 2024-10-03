#include "LinearInterpolator.h"


LinearInterpolator::LinearInterpolator(){
    //not sure i need to do anything here
    }

int LinearInterpolator::calculate_distance(int raw_value){
    if(raw_value <= this->raw_values[0]){
        return this->mm_values[0];
    } else if(raw_value >= this->raw_values[56-1]){
        return this->mm_values[56-1];
    } else{
        int lower_index = 0;
        for(int itr = 1; itr < 56; itr++){
            if(raw_value < this->raw_values[itr]){
                lower_index = itr - 1;
                break;
            }
        }
        float gradient = float(mm_values[lower_index + 1] - mm_values[lower_index]) / (raw_values[lower_index + 1] - raw_values[lower_index]);
        int interpolated_mm = this->mm_values[lower_index] + (raw_value - raw_values[lower_index]) * gradient;
        return interpolated_mm;

    }
        

}