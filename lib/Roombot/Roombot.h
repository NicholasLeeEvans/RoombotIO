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
        
        void turn_angle(float angle);
        
        void spin_once(int direction);

        void move_forward(int distance);
        
        int get_wheelbase(){return this->wheel_base;};
        void set_wheelbase(int _wheelbase){this->wheel_base = _wheelbase;};
        
        int get_wheeldiam(){return this->wheel_diam;};
        void set_wheeldiam(int _wheeldiam){this->wheel_diam = _wheeldiam;};

        void spin_and_scan();
        
    private:
        Stepper *stepper_left;
        Stepper *stepper_right;
        int rpm;
        RangeFinder *front_range;
        //RangeFinder *left_range;
        //RangeFinder *right_range;
        
        int wheel_base;
        int wheel_diam;
        
        //coordinate system, x and y in mm, angle where it would be pointing.
        int location_x;
        int location_y;
        float angle;

        int scanned_values[];
};


#endif