#include <Arduino.h>
//#include "BluetoothSerial.h"

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
void IRAM_ATTR Timer1_ISR()
{
  //stepper_left.step_once();
  my_roombot.increment_step_count(stepper_left.step_once(),-1);
}

void IRAM_ATTR Timer2_ISR()
{
  //stepper_right.step_once();
  my_roombot.increment_step_count(stepper_right.step_once(),1);
}




void setup() {

  Serial.begin(115200);
  Serial.println("serial started");
  delay(10);

  Serial.println("setting timers");
  stepper_left.set_timer(1, &Timer1_ISR);
  stepper_right.set_timer(2, &Timer2_ISR);
  delay(100);

  Serial.println("starting serialBT");
  my_roombot.init_serialBT();
  //SerialBT.begin(device_name);
  delay(10);
  //Serial.print("step to angle ratio:");
  //Serial.println(my_roombot.get_step_to_angle_ratio());
  my_roombot.set_rpm(10); // this will msg twice, once for each stepper
  delay(10);
  Serial.println("entering loop: ");
}


void loop() {
  //checks the bluetooth serial input and then moves based on wasd controls
  my_roombot.checkBTcommands();
  my_roombot.update_position();
  delay(50);
}




