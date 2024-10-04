#ifndef ROOMBOT_H
#define ROOMBOT_H

#include <Arduino.h>
#include "Stepper.h"
#include "RangeFinder.h"
#include "LinearInterpolator.h"

class Roombot {
    private:
        Stepper *stepper_left;
        Stepper *stepper_right;
        int rpm; //probably change this to float later for more precision, might be pointless though
        RangeFinder *front_range;
        int front_range_offset;
        //RangeFinder *left_range;
        //RangeFinder *right_range;

        LinearInterpolator my_interpolator;
        
        int wheel_base;
        int wheel_diam;
        int wheel_circumference;

        //step counters
        int counter_left;
        int counter_right;

        int last_left;
        int last_right;
        
        //coordinate system, x and y in mm, angle where it would be pointing.
        float location_x;
        float location_y;
        float angle;
        float step_to_angle_ratio;

        int scanned_values[];

    public:
        Roombot(Stepper *left, Stepper *right, RangeFinder *front);
        void reset_x_y_angle();
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

        int scan_once();
        void spin_and_scan();

        float get_angle(){return this->angle;};
        float get_position_x(){return this->location_x;};
        float get_position_y(){return this->location_y;};
        
        void print_location_angle();

        float get_step_to_angle_ratio(){return this->step_to_angle_ratio;};
        
        void update_position();

        int interpolate_value(int value){return my_interpolator.calculate_distance(value);};
        
}; 



#endif