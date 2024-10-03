#include <Arduino.h>
#include "BluetoothSerial.h"

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

String device_name = "ESP32-BT-Slave";
BluetoothSerial SerialBT;

void setup() {

  Serial.begin(115200);
  Serial.println("serial started");
  delay(10);

  Serial.println("setting timers");
  stepper_left.set_timer(1, &Timer1_ISR);
  stepper_right.set_timer(2, &Timer2_ISR);
  delay(100);

  Serial.println("starting serialBT");

  SerialBT.begin(device_name);
  delay(10);
  //Serial.print("step to angle ratio:");
  //Serial.println(my_roombot.get_step_to_angle_ratio());
  my_roombot.set_rpm(10); // this will msg twice, once for each stepper
  delay(10);
}

void check_BT_commands();

void loop() {
  //checks the bluetooth serial input and then moves based on wasd controls
  check_BT_commands();
}


void check_BT_commands(){
  if (SerialBT.available()) {
    char message = (SerialBT.read());
    Serial.print("received: ");
    Serial.println(message);
    int new_rpm;
    switch(message){
      case 'w':
        Serial.println("moving forward");
        my_roombot.move_forward(100);
        break;
      case 's':
        Serial.println("moving backward");
        my_roombot.move_forward(-100);
        break;
      case 'a':
        Serial.println("turning left");
        my_roombot.turn_angle(45);
        break;
      case 'd':
        Serial.println("turning right");
        my_roombot.turn_angle(-45);
        break;
      case 'j':
        Serial.print("speeding up to: ");
        new_rpm = min(my_roombot.get_rpm() + 2,16);
        Serial.print(new_rpm);
        Serial.println("rpm");
        my_roombot.set_rpm(min(my_roombot.get_rpm() + 2,20));
        break;
      case 'k':
        Serial.print("slowing down to: ");
        new_rpm = max(my_roombot.get_rpm() - 2,2);
        Serial.print(new_rpm);
        Serial.println("rpm");
        my_roombot.set_rpm(max(my_roombot.get_rpm() - 2,2));
        break;
      case 'z':
        Serial.println("spin and scan: ");
        my_roombot.spin_and_scan();
        break;
      case 'x':
        Serial.println("one scan: ");
        my_roombot.scan_once();
        break;
      default:
        Serial.println("unknown message received :(");
        break;  
    }

  }

  my_roombot.update_position();

  delay(50);
}

