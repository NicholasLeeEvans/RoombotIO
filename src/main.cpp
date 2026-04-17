#include <Arduino.h>

#include "Stepper.h"
#include "RangeFinder.h"
#include "Roombot.h"

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

const char* ssid = "aterm-326c63-g";
const char* password = "282818d307488";

void setup_wifi(const char* ssid, const char* password) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi connected!");
    Serial.println(WiFi.localIP());
}

AsyncWebServer server(80);
AsyncEventSource events("/events");

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

  setup_wifi(ssid, password);

  server.on("/action", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->hasParam("type")){
        request->send(400, "application/json", "{\"error\":\"missing type\"}");
        return;
    }
    String type = request->getParam("type")->value();
    Command cmd;
    
    if(type == "straight"){
      if(!request->hasParam("distance")) {
        request->send(400, "application/json", "{\"error\":\"missing distance\"}");
       return;
      }
      int distance = request->getParam("distance")->value().toInt();
      cmd.type = Command::STRAIGHT;
      cmd.params.straight.distance = distance;
      Serial.print("moving ");
      Serial.print(distance);
      Serial.println("mm"); 
    } else if(type == "turn"){
      if(!request->hasParam("angle")){
        request->send(400, "application/json", "{\"error\":\"missing turn angle\"}");
       return;
      }
      int angle = request->getParam("angle")->value().toInt();
      int radius = request->hasParam("radius") ? request->getParam("radius")->value().toInt() : 0;
      cmd.type = Command::ARC_TURN;
      cmd.params.arc_turn.angle = angle;
      cmd.params.arc_turn.radius = radius;
      Serial.print("turning ");
      Serial.print(angle);
      Serial.println("degrees");
    } else if(type == "speed"){
      if(!request->hasParam("set_rpm")){
        request->send(400, "application/json", "{\"error\":\"missing rpm\"}");
       return;
      }
      int rpm = request->getParam("set_rpm")->value().toInt();
      cmd.type = Command::SET_RPM;
      cmd.params.set_rpm.rpm = rpm;
    } else if(type == "status"){
      cmd.type = Command::STATUS;
      StatusData status = my_roombot.get_status();
      String response = "{\"x\":" + String(status.x) + ",\"y\":" + String(status.y) 
                      + ",\"angle\":" + String(status.angle) + ",\"range\":" + String(status.range) + "}";
      request->send(200, "application/json", response);
      return;  // Don't call execute_command for STATUS
    } else {
      request->send(400, "application/json", "{\"error\": " + type + "\"}");
      return;
    }
      
    my_roombot.execute_command(cmd);
    request->send(200, "application/json", "{\"status\":\"ok\"}");
    
  });

  server.addHandler(&events);

  server.begin();
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
  Serial.println("HTTP server started");

  my_roombot.set_rpm(10); 
  delay(10);

  Serial.println("setting timers");
  stepper_left.set_timer(1, &Timer_ISR_Left);
  stepper_right.set_timer(2, &Timer_ISR_Right);
  delay(100);

  range_front.set_timer(3, &Timer_ISR_RangeFinder);
  
  Serial.println("entering loop: ");

  last_update = millis();
}


void loop() {
  my_roombot.update_position();

  int distance = my_roombot.scan_once();
      
  events.send(String(distance).c_str(), "range");
  delay(50);
}




