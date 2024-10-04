#include "Roombot.h"

#define PI_OVER_180 0.01745329

Roombot::Roombot(Stepper *left, Stepper *right, RangeFinder *front){
    int initial_rpm = 10; // probably set the max rpm to 16, was getting only one working at 18rpm, and none at 20rpm
    // set up initial location
    this->reset_x_y_angle();

    //initial step counters
    this->counter_left = 0;
    this->counter_right = 0;
    this->last_left = 0;
    this->last_right = 0;
    

    //based off prototype, in mm
    this->wheel_base = 120;
    this->wheel_diam = 62;
    this->wheel_circumference = this->wheel_diam * 3.1415;

    //attach steppers
    this->stepper_left = left;
    this->stepper_right = right;
    
    set_rpm(initial_rpm);

    //set up rangefinder
    this->front_range = front;
    this->front_range_offset = 70;

    this->my_interpolator = LinearInterpolator();

    this->step_to_angle_ratio = 180.0f * this->wheel_diam / this->wheel_base / stepper_left->get_steps_per_rev();
}

void Roombot::reset_x_y_angle(){
    this->location_x = 0;
    this->location_y = 0;
    this->angle = 0.0f;
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
    float wheel_rotations = float(distance) / this->wheel_circumference;
    int steps_needed = wheel_rotations * stepper_left->get_steps_per_rev();
    Serial.println(steps_needed);
    this->stepper_left->manual_steps(steps_needed);
    this->stepper_right->manual_steps(steps_needed);
}

void Roombot::increment_step_count(int _step, int _side){
    //no floating point calcs in an interrupt!!
    if(_side == -1){
        this->counter_left += _step;
    }else{
        this->counter_right += _step;
    }
}

int Roombot::scan_once(){
    int scan_value = this->front_range->get_range();
    Serial.print("raw val: ");
    Serial.print(scan_value);
    int guess_mm = this->my_interpolator.calculate_distance(scan_value);
    Serial.print(", estimated distance: ");
    Serial.print(guess_mm);

    //estimate the position of the scan, add the offset from center to range finder. other will need to have angle offset
    int range_x = this->location_x + (this->front_range_offset + guess_mm * cos(this->angle * PI_OVER_180));
    int range_y = this->location_y + (this->front_range_offset + guess_mm * sin(this->angle * PI_OVER_180));

    Serial.print(", (x, y): (");
    Serial.print(range_x);
    Serial.print(", ");
    Serial.print(range_y);
    Serial.println(")");

    return scan_value;
}

void Roombot::spin_and_scan(){
    //set the turn angle to 360, and start reading the rangefinder
    //need a variable to store the values somewhere...
    spin_once(1);
    delay(5);
    while((this->stepper_left->get_steps_remaining() > 0) || (this->stepper_left->get_steps_remaining() > 0)){
        this->update_position();
        this->scan_once();
        delay(50);
    }
}

void Roombot::print_location_angle(){
    Serial.print("(x, y): (");
    Serial.print(int(this->get_position_x()));
    Serial.print(", ");
    Serial.print(int(this->get_position_y()));
    Serial.print("), angle: ");
    Serial.println(int(this->get_angle()));
}

void Roombot::update_position(){
    //calculates the cartesian coords and angle of the robot
    
    //not sure but i might need to save these values at the start so i can make sure they arent updated while i do calcs...
    noInterrupts();
    int steps_left = this->counter_left;
    int steps_right = this->counter_right;
    interrupts();

    int delta_steps = (steps_right - steps_left);
    //Serial.print("delta steps:");
    //Serial.println(delta_steps);
    this->angle = fmod(this->step_to_angle_ratio * delta_steps, 360.0);

    //calculate the steps done since last cartesian coord update
    int new_left_steps = steps_left - last_left;
    int new_right_steps = steps_right - last_right;
    
    //update the last checked values
    this->last_left = steps_left;
    this->last_right = steps_right;

    int new_linear_steps = (new_left_steps + new_right_steps) / 2;
    float distance_travelled = new_linear_steps * (float(this->wheel_circumference) / stepper_left->get_steps_per_rev());

    float d_x = distance_travelled * cos(this->angle * PI_OVER_180);
    float d_y = distance_travelled * sin(this->angle * PI_OVER_180);

    this->location_x += d_x;
    this->location_y += d_y;

}