#include "Roombot.h"

Roombot::Roombot(Stepper *left, Stepper *right, RangeFinder *front){
    int initial_rpm = 10; // probably set the max rpm to 16, was getting only one working at 18rpm, and none at 20rpm
    // set up initial location
    this->location_x = 0;
    this->location_y = 0;
    this->angle = 0.0f;

    //initial step counters
    this->counter_left = 0;
    this->counter_right = 0;

    //based off prototype, in mm
    this->wheel_base = 120;
    this->wheel_diam = 62;

    //attach steppers
    this->stepper_left = left;
    this->stepper_right = right;
    
    set_rpm(initial_rpm);

    //set up rangefinder
    this->front_range = front;

    this->step_to_angle_ratio = 180.0f * this->wheel_diam / this->wheel_base / stepper_left->get_steps_per_rev();
}

void Roombot::set_rpm(int rpm){
    this->rpm = rpm;
    this->stepper_left->set_speed(rpm);
    this->stepper_right->set_speed(rpm);
}

//positive angle is left turn
void Roombot::turn_angle(float turn_angle){
    //convert the angle to steps to send to the stepper motors.
    float wheel_rotations = turn_angle * wheel_base / wheel_diam / 360;
    int steps_needed = wheel_rotations * stepper_left->get_steps_per_rev();
    
    this->stepper_left->manual_steps(-steps_needed);
    this->stepper_right->manual_steps(steps_needed);
    
}

void Roombot::spin_once(int direction){
    //set left positive
    this->turn_angle(direction * 360);
    
}

//distance is in mm, should automatically take care of negative numbers
void Roombot::move_forward(int distance){
    float wheel_circumference = 3.14 * wheel_diam; //about 180
    float wheel_rotations = distance / wheel_circumference;
    int steps_needed = wheel_rotations * stepper_left->get_steps_per_rev();
    Serial.println(steps_needed);
    this->stepper_left->manual_steps(steps_needed);
    this->stepper_right->manual_steps(steps_needed);

    //update the location
    int d_x = distance * cos(this->angle * 3.14 / 180);
    int d_y = distance * sin(this->angle * 3.14 / 180);
    
    this->location_x += d_x;
    this->location_y += d_y;
    Serial.print("Moving to: (");
    Serial.print(this->location_x);
    Serial.print(", ");
    Serial.print(this->location_y);
    Serial.println(")");  
         
}

void Roombot::increment_step_count(int _step, int _side){
    if(_side == -1){
        this->counter_left += _step;
        
    }else{
        this->counter_right += _step;
    }
    //no floating point calcs in an interrupt!!

}

void Roombot::spin_and_scan(){
    //set the turn angle to 360, and start reading the rangefinder
    //need a variable to store the values somewhere...
    spin_once(1);

}

/*
int* Roombot::get_position(){
    int position[2];
    position[0] = this->location_x;
    position[1] = this->location_y;
    return position;
}
*/

void Roombot::update_position(){
    //noInterrupts();
    int delta_steps = (this->counter_right - this->counter_left); // might be fine without disabling interrupts
    this->angle = fmod(this->step_to_angle_ratio * delta_steps, 360.0);

    //interrupts();
    //Serial.print("delta steps:");
    //Serial.println(delta_steps);
    
    //need to figure out how to do the linear stuff next maybe save the last checked steps
}