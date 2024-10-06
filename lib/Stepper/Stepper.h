#ifndef STEPPER_H
#define STEPPER_H
#include <Arduino.h>

class Stepper {

public:
    Stepper(int _steps_per_rev, int _pin1, int _pin2, int _pin3, int _pin4);
    void set_timer(int timer_number, void (*isr)());

    void setup_pins();
    void set_rpm(int _rpm);
    int step_once();
    //void update_stepper();
    void manual_steps(int steps);
    int get_steps_remaining(){return this->steps_remaining;};
    int get_steps_per_rev(){return this->steps_per_rev;};
    unsigned int get_step_interval_us(){return this->step_interval_us;};
    
private:
    int motor_pin1; //blue
    int motor_pin2; //pink
    int motor_pin3; //yellow
    int motor_pin4; //orange
    int steps_per_rev;
    int speed_rpm;
    volatile int steps_remaining; // will need to sort this out for continuous mode, probably just put an if statement in step_once
    unsigned int step_interval_us;
    volatile int current_state;
    int direction; // ccw = 1, cw = -1 (ccw is given in datasheet)
    hw_timer_t *Timer_cfg;

    void write_pins(int motor_state_1,int motor_state_2,int motor_state_3,int motor_state_4);
    void drive_pins(int _state);
    
};

#endif