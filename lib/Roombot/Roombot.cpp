#include "Roombot.h"
//#include <math.h>

Roombot::Roombot(Stepper *left, Stepper *right, RangeFinder *front){
    int initial_rpm = 10; // probably set the max rpm to 16, was getting only one working at 18rpm, and none at 20rpm
    // set up initial location
    this->location_x = 0;
    this->location_y = 0;
    this->angle = 0.0f;

    //based off prototype, in mm
    this->wheel_base = 120;
    this->wheel_diam = 62;

    //attach steppers
    this->stepper_left = left;
    this->stepper_right = right;
    
    set_rpm(initial_rpm);
    //this->stepper_left->set_speed(rpm);
    //this->stepper_right->set_speed(rpm);

    //set up rangefinder
    this->front_range = front;
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

    //TODO: need a way to see how much it actually turned if it gets interrupted. but until then...
    this->angle += turn_angle;
    Serial.print("Rotating to: ");
    Serial.print(this->angle);
    Serial.println("deg");
    
    
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