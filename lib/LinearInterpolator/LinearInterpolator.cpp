#include "LinearInterpolator.h"

#define TOTAL_DIVISIONS 56

LinearInterpolator::LinearInterpolator(){
    //precompute gradients once on build
    for(int itr = 0; itr < TOTAL_DIVISIONS - 1; itr++)
    {
        gradients[itr] = float(mm_values[itr + 1] - mm_values[itr]) / (raw_values[itr + 1] - raw_values[itr]);
        
    }
    
    }

int LinearInterpolator::calculate_distance(int raw_value){
    if(raw_value <= this->raw_values[0]){
        return this->mm_values[0];
    } else if(raw_value >= this->raw_values[TOTAL_DIVISIONS - 1]){
        return this->mm_values[TOTAL_DIVISIONS - 1];
    } else{
        int lower_index = 0;
        for(int itr = 1; itr < TOTAL_DIVISIONS; itr++){
            if(raw_value < this->raw_values[itr]){
                lower_index = itr - 1;
                break;
            }
        }
        float gradient = gradients[lower_index]; 
        int interpolated_mm = this->mm_values[lower_index] + (raw_value - raw_values[lower_index]) * gradient;
        return interpolated_mm;

    }
        

}