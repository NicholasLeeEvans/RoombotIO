#include <Arduino.h>

#include "Stepper.h"
#include "RangeFinder.h"
#include "Roombot.h"

#define IR_RANGE_PIN 36

RangeFinder range_front(IR_RANGE_PIN);

//these values below should be set up for +ve directions being going forward for both
Stepper stepper_left(4096,27,14,12,13);
Stepper stepper_right(4096,26,25,33,32);

Roombot my_roombot(&stepper_left, &stepper_right, &range_front);

//would like to remove these and put in the class somehow... maybe reference the timer with a 1 and 2 in the roombot class and do it there?
void IRAM_ATTR Timer_ISR_Left()
{
  my_roombot.increment_step_count(stepper_left.step_once(),-1);
}

void IRAM_ATTR Timer_ISR_Right()
{
  my_roombot.increment_step_count(stepper_right.step_once(),1);
}

void IRAM_ATTR Timer_ISR_RangeFinder()
{
  range_front.take_multiple_readings(7);
}

unsigned long last_update; // should be unsigned long
#define POSITION_UPDATE_MS 1000

void setup() {

  Serial.begin(115200);
  Serial.println("serial started");
  delay(10);

  Serial.println("setting timers");
  stepper_left.set_timer(1, &Timer_ISR_Left);
  stepper_right.set_timer(2, &Timer_ISR_Right);
  delay(100);

  range_front.set_timer(3, &Timer_ISR_RangeFinder);

  Serial.println("starting serialBT");
  my_roombot.init_serialBT(); //want to change this to USBNOW
  delay(10);
  my_roombot.set_rpm(10); 
  delay(10);
  Serial.println("entering loop: ");

  last_update = millis();
}


void loop() {
  //checks the bluetooth serial input and then moves based on wasd controls
  my_roombot.checkBTcommands();
  my_roombot.update_position();
  unsigned long now = millis();
  if(now > (last_update + POSITION_UPDATE_MS)){
    last_update = now;
    //reports the x,y,angle, and range detected so this can be put into the map building
    Serial.print(int(my_roombot.get_position_x()));
    Serial.print(",");
    Serial.print(int(my_roombot.get_position_y()));
    Serial.print(",");
    Serial.print(int(my_roombot.get_angle()));
    Serial.print(",");
    Serial.println(int(my_roombot.scan_once()));

  }
  
  delay(50);
}




