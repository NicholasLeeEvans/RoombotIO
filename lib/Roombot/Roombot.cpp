#include "Roombot.h"

#define INITIAL_RPM 10
#define RPM_LIMIT   16

#define WHEELBASE       120
#define WHEEL_DIAMETER  62

#define FRONT_RANGEFINDER_OFFSET  70
#define MAX_RANGEFINDER_SCAN_DIST 300

#define FORWARD_COMMAND_DIST  100
#define TURN_COMMAND_ANGLE    45

Roombot::Roombot(Stepper *_stepper_left, Stepper *_stepper_right, RangeFinder *_front_range){
    
    
    int initial_rpm = INITIAL_RPM; // probably set the max rpm to 16, was getting only one working at 18rpm, and none at 20rpm
    this->rpm_limit = RPM_LIMIT;
    // set up initial location
    this->reset_x_y_angle();

    //initial step counters
    this->counter_left = 0;
    this->counter_right = 0;
    this->last_left = 0;
    this->last_right = 0;

    //based off prototype, in mm
    this->wheel_base = WHEELBASE;
    this->wheel_diam = WHEEL_DIAMETER;
    this->wheel_circumference = this->wheel_diam * PI;

    //attach steppers
    this->stepper_left = _stepper_left;
    this->stepper_right = _stepper_right;
    
    set_rpm(initial_rpm);

    //set up rangefinder
    this->front_range = _front_range;
    this->front_range_offset = FRONT_RANGEFINDER_OFFSET;

    this->my_interpolator = LinearInterpolator();

    this->step_to_angle_ratio = (180.0f * this->wheel_diam) / this->wheel_base / stepper_left->get_steps_per_rev();
    
    
    delay(10);
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

void Roombot::set_rpms(int rpm_left, int rpm_right){
    this->stepper_left->set_rpm(rpm_left);
    this->stepper_right->set_rpm(rpm_right);
}

void Roombot::set_rpm(int rpm){
  if(rpm > this->rpm_limit){
    rpm = this->rpm_limit;
  } else if(rpm < 1){
    rpm = 1;
  }
    this->rpm = rpm;
    this->set_rpms(rpm, rpm);
}

//positive angle is left turn
void Roombot::turn_angle(float turn_angle, int turn_radius){
  float turn_angle_rad = (turn_angle * PI / 180);
  int turn_sign = (turn_angle_rad > 0) - (turn_angle_rad < 0);
  int left_radius = turn_radius - turn_sign * (wheel_base/2);
  int right_radius = turn_radius + turn_sign * (wheel_base/2);
  
  float left_arc = abs(turn_angle_rad) * left_radius;
  float right_arc = abs(turn_angle_rad) * right_radius;
  
  float left_wheel_rotations = left_arc / this->wheel_circumference;
  float right_wheel_rotations = right_arc / this->wheel_circumference;
  
  int left_steps_needed = left_wheel_rotations * stepper_left->get_steps_per_rev();
  int right_steps_needed = right_wheel_rotations * stepper_right->get_steps_per_rev();
  
  //set the rpms to the correct ratio for smooth turning.
  //need to calculate ratio based on left or right... rpm is always positive.
  // left side, right steps should be denominator,
  if((left_steps_needed != 0) && (right_steps_needed != 0)){
    if(turn_angle > 0){ //left turn
      float ratio = abs(float(left_steps_needed) / right_steps_needed);
      int ratio_rpm = max(int(ratio * (this->rpm)), 1);
      this->set_rpms(ratio_rpm, this->rpm);
    } else if(turn_angle < 0){ //right turn
    
      float ratio = abs(float(right_steps_needed) / left_steps_needed);
      int ratio_rpm = max(int(ratio * (this->rpm)), 1);
      this->set_rpms(this->rpm, ratio_rpm);
    } //no else, shouldnt be 0 turn radius
  }
  
  
  this->stepper_left->manual_steps(left_steps_needed);
  this->stepper_right->manual_steps(right_steps_needed);
}

//positive angle is left turn, do zero radius turn (on the spot)
void Roombot::turn_angle(float turn_angle){
  this->set_rpm(this->rpm);
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
  this->set_rpm(this->rpm);  
  float wheel_rotations = float(distance) / this->wheel_circumference;
  int steps_needed = wheel_rotations * stepper_left->get_steps_per_rev();
  //Serial.println(steps_needed);
  this->stepper_left->manual_steps(steps_needed);
  this->stepper_right->manual_steps(steps_needed);
}

void Roombot::increment_step_count(int _step, int _side){
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

    if(calculated_mm_distance < MAX_RANGEFINDER_SCAN_DIST){
      //estimate the position of the scan, add the offset from center to range finder. other will need to have angle offset
      int range_x = this->location_x + ((this->front_range_offset + calculated_mm_distance) * cos(this->angle * DEG_TO_RAD));
      int range_y = this->location_y + ((this->front_range_offset + calculated_mm_distance) * sin(this->angle * DEG_TO_RAD));
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
    while((this->stepper_left->get_steps_remaining() > 0) || (this->stepper_right->get_steps_remaining() > 0)){
        this->update_position();
        this->scan_once();
        delay(200);
    }
    this->set_rpm(old_rpm);
}

void Roombot::print_location_angle(){
  //TODO replace the BT code that was removed
}

StatusData Roombot::get_status(){
  return {int(this->get_position_x()),
          int(this->get_position_y()),
          int(this->get_angle()),
          int(this->scan_once())
  };
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

    float d_x = distance_travelled * cos(this->angle * DEG_TO_RAD);
    float d_y = distance_travelled * sin(this->angle * DEG_TO_RAD);

    this->location_x += d_x;
    this->location_y += d_y;

}

void Roombot::execute_command(Command cmd)
{
  switch(cmd.type){
    case Command::STRAIGHT:{
      this->move_forward(cmd.params.straight.distance);
      break;
    }

    case Command::ARC_TURN:{
      int turn_radius = cmd.params.arc_turn.radius;
      this->turn_angle((float)cmd.params.arc_turn.angle, turn_radius);
      break;
    }

    case Command::SET_RPM:{
      this->set_rpm(cmd.params.set_rpm.rpm);
      break;
    }

    case Command::STOP:
      break;

    case Command::STATUS:
      this->print_location_angle();
      break;

    case Command::SCAN:
      this->scan_once();
      break;
    
    default:
      break;
    
    
    
    
    
  }
}

  