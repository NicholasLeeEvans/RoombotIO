#include "Stepper.h"

Stepper::Stepper(int _steps_per_rev, int _pin1, int _pin2, int _pin3, int _pin4)
    {
      this->Timer_cfg = NULL;
      int initial_rpm = 10;
      this->steps_per_rev = _steps_per_rev;
      this->motor_pin1 = _pin1;
      this->motor_pin2 = _pin2;
      this->motor_pin3 = _pin3;
      this->motor_pin4 = _pin4;
      this->steps_remaining = 0;
      this->current_state = 0; //all off
      this->direction = 1;
      setup_pins(); //sort out pinMode
      set_speed(initial_rpm);
      drive_pins(0); //start with nothing
      
    }

void Stepper::set_timer(int timer_number, void (*isr)())
{
  //set the timer to 1Mhz
  //might need to make sure this equates to 1Mhz because all of the interrupt timing is based on that
  Timer_cfg = timerBegin(timer_number, 80, true); 
  timerAttachInterrupt(Timer_cfg, isr, true);
  //timerAlarmWrite(Timer_cfg, 500, true);
  timerAlarmWrite(Timer_cfg, this->step_interval_us, true); //for variable interrupt timing
  timerAlarmEnable(Timer_cfg);

  //Serial.println("disabling stepper in set_timer");
  //timerAlarmDisable(Timer_cfg); //start with the timer disabled until required
  
}

void Stepper::step_once()
{
    current_state += direction;
    if(current_state > 8)
      current_state = 1;
    else if(current_state < 1)
      current_state = 8;
    drive_pins(current_state);
    
    --steps_remaining;
}
void Stepper::update_stepper()
    {
      /*
      this function is called via hw timer interrupt
      will loop stepping once, then disable the interrupt timer when it is finished
      the timer is re-enabled when needed in the manual_steps function

      */
      
      if((steps_remaining > 0)){ 
        step_once();
      } else{
        //timerAlarmDisable(Timer_cfg);
        //Serial.println("disabling stepper in update_stepper");
      } 

    }

void Stepper::manual_steps(int steps)
{
  //should probably think about the case where steps == 0
  /*
  if(this->steps_remaining <= 0){
    //if the previous steps_remaining was 0, then the timer would have been disabled
    Serial.println("enabling stepper in manual steps");
    timerAlarmWrite(Timer_cfg, step_interval_us, true);
    timerAlarmEnable(Timer_cfg);
  }
  */
  
  
  direction = 1;
  if(steps < 0){
    steps = abs(steps);
    direction = -1;
  }
    
  this->steps_remaining = steps;
}

void Stepper::setup_pins()
    {
      //make sure all the pins are configured as outputs
      pinMode(motor_pin1, OUTPUT);
      pinMode(motor_pin2, OUTPUT);
      pinMode(motor_pin3, OUTPUT);
      pinMode(motor_pin4, OUTPUT);
    }

void Stepper::write_pins(int motor_state_1,int motor_state_2,int motor_state_3,int motor_state_4)
{
    digitalWrite(motor_pin1, motor_state_1); //blue
    digitalWrite(motor_pin2, motor_state_2); //pink
    digitalWrite(motor_pin3, motor_state_3); //yellow
    digitalWrite(motor_pin4, motor_state_4); //orange
}

void Stepper::drive_pins(int _state)
    {
      switch(_state){
        case 0:
            write_pins(LOW, LOW, LOW, LOW);
          break;
        case 1:
            write_pins(LOW, LOW, LOW, HIGH);
          break;
        case 2:
            write_pins(LOW, LOW, HIGH, HIGH);
          break;
        case 3:
            write_pins(LOW, LOW, HIGH, LOW);
          break;
        case 4:
            write_pins(LOW, HIGH, HIGH, LOW);
          break;
        case 5:
            write_pins(LOW, HIGH, LOW, LOW);
          break;
        case 6:
            write_pins(HIGH, HIGH, LOW, LOW);
          break;
        case 7:
            write_pins(HIGH, LOW, LOW, LOW);
          break;
        case 8:
            write_pins(HIGH, LOW, LOW, HIGH);
          break;
        default:
            write_pins(LOW, LOW, LOW, LOW);
          break;
      }
    }

void Stepper::set_speed(int _rpm){
    //should check for limit here... max 20, min -20? if negative then set the interval as abs(rpm) and set direction -ve
    this->speed_rpm = _rpm;
    this->step_interval_us = (60L * 1000L * 1000L) / steps_per_rev / speed_rpm;

    //update to variable interrupt timing:
    if(Timer_cfg != NULL){
      //timerAlarmWrite(Timer_cfg, 500, true);
      Serial.print("setting alarm write to: ");
      Serial.println(step_interval_us);
      
      timerAlarmWrite(Timer_cfg, step_interval_us, true);
    }
}