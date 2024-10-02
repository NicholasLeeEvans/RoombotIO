#ifndef ROOMBOT_H
#define ROOMBOT_H

#include <Arduino.h>
#include "Stepper.h"
#include "RangeFinder.h"

class Roombot {
    public:
        Roombot(Stepper *left, Stepper *right, RangeFinder *front);
        void set_rpm(int rpm);
        int get_rpm(){return this->rpm;};

        int get_wheelbase(){return this->wheel_base;};
        void set_wheelbase(int _wheelbase){this->wheel_base = _wheelbase;};
        
        int get_wheeldiam(){return this->wheel_diam;};
        void set_wheeldiam(int _wheeldiam){this->wheel_diam = _wheeldiam;};
        
        void turn_angle(float angle);
        void spin_once(int direction);

        void move_forward(int distance);

        void increment_step_count(int _step, int _side);

        void spin_and_scan();

        float get_angle(){return this->angle;};
        //int* get_position();
        float get_step_to_angle_ratio(){return this->step_to_angle_ratio;};
        
        void update_position();
        
        
    private:
        Stepper *stepper_left;
        Stepper *stepper_right;
        int rpm; //probably change this to float later for more precision, might be pointless though
        RangeFinder *front_range;
        //RangeFinder *left_range;
        //RangeFinder *right_range;
        
        int wheel_base;
        int wheel_diam;

        //step counters
        int counter_left;
        int counter_right;
        
        //coordinate system, x and y in mm, angle where it would be pointing.
        int location_x;
        int location_y;
        float angle;
        float step_to_angle_ratio;

        int scanned_values[];
};


#endif