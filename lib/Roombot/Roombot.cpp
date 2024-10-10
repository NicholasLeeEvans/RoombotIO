#include "Roombot.h"

#define PI_OVER_180 0.01745329

Roombot::Roombot(Stepper *left, Stepper *right, RangeFinder *front){
    
    
    int initial_rpm = 10; // probably set the max rpm to 16, was getting only one working at 18rpm, and none at 20rpm
    this->rpm_limit = 16;
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

    this->step_to_angle_ratio = (180.0f * this->wheel_diam) / this->wheel_base / stepper_left->get_steps_per_rev();
    
    
    delay(10);
}

void Roombot::init_serialBT(){
    this->SerialBT.begin("Roombot");
}

void Roombot::reset_x_y_angle(){
    this->location_x = 0;
    this->location_y = 0;

    /*
    this doesnt work as it is updated again based on steps.
    this->angle = 0.0f;
    */
    //just reset the counters and angle will be sorted out. need to clear 'last steps' too otherwise xy will break
    this->counter_left = 0;
    this->counter_right = 0;
    this->last_left = 0;
    this->last_right = 0;
    
}

void Roombot::set_rpm(int rpm){
  if(rpm > this->rpm_limit){
    rpm = this->rpm_limit;
  } else if(rpm < 1){
    rpm = 1;
  }
    this->rpm = rpm;
    this->stepper_left->set_rpm(rpm);
    this->stepper_right->set_rpm(rpm);
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
    //probably need to ignore anything too large right now...
    int scan_value = this->front_range->get_range_value();
    int calculated_mm_distance = this->my_interpolator.calculate_distance(scan_value);


    //ignore big values as they are pretty sketchy...

    if(calculated_mm_distance < 300){
      //estimate the position of the scan, add the offset from center to range finder. other will need to have angle offset
      int range_x = this->location_x + ((this->front_range_offset + calculated_mm_distance) * cos(this->angle * PI_OVER_180));
      int range_y = this->location_y + ((this->front_range_offset + calculated_mm_distance) * sin(this->angle * PI_OVER_180));

      SerialBT.print(range_x);
      SerialBT.print(",");
      SerialBT.println(range_y);
    } else {
      SerialBT.print("returned val greater than 300mm: ");
      SerialBT.println(calculated_mm_distance);
    }
    
    //..still return the guess
    return calculated_mm_distance;
}

void Roombot::spin_and_scan(){
    //set the turn angle to 360, and start reading the rangefinder
    //need a variable to store the values somewhere...
    spin_once(1);
    delay(5);
    int old_rpm = this->get_rpm();
    this->set_rpm(5);
    while((this->stepper_left->get_steps_remaining() > 0) || (this->stepper_left->get_steps_remaining() > 0)){
        this->update_position();
        this->scan_once();
        delay(200);
    }
    this->set_rpm(old_rpm);
}

void Roombot::print_location_angle(){
    SerialBT.print("(x, y): (");
    SerialBT.print(int(this->get_position_x()));
    SerialBT.print(", ");
    SerialBT.print(int(this->get_position_y()));
    SerialBT.print("), angle: ");
    SerialBT.println(int(this->get_angle()));
}

void Roombot::update_position(){
    //calculates the cartesian coords and angle of the robot, basically this overrides any angle clearing code.
    
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

void Roombot::checkBTcommands(){
    if (this->SerialBT.available()) {
    char message = (SerialBT.read());
    //Serial.print("received: ");
    //Serial.println(message);
    int new_rpm;
    switch(message){
      case 'w':
        //Serial.println("moving forward");
        this->move_forward(100);
        break;
      case 's':
        //Serial.println("moving backward");
        this->move_forward(-100);
        break;
      case 'a':
        //Serial.println("turning left");
        this->turn_angle(45);
        break;
      case 'd':
        //Serial.println("turning right");
        this->turn_angle(-45);
        break;
      case 'j':
        this->set_rpm(this->get_rpm() + 1);
        break;
      case 'k':
        this->set_rpm(this->get_rpm() - 1);
        break;
      case 'z':
        //Serial.println("spin and scan: ");
        this->spin_and_scan();
        break;
      case 'x':
        //Serial.println("one scan: ");
        this->scan_once();
        break;
      case 'c':
        //Serial.println("resetting x,y coords and angle: ");
        this->reset_x_y_angle();
        break;
      case 'e':
        this->print_location_angle();
        break;
      default:
        //Serial.println("unknown message received :(");
        break;  
    } //end switch
  } //end if serialBT available

}

  