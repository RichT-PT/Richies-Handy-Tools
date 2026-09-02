/*
  Author: Bonezegei (Jofel Batutay)
  Date: July 2023 
  XPT2046 getPoints 
  
  ------------------------
  |  LCD PIN   |  ESP32  |
  | MOSI       |   23    |
  | SCK        |   18    |
  | MISO       |   19    | 
  | TS_CS      |   33    | Can be Assign to other pin
  ------------------------

  [ IMPORTANT ] Install with Bonezegei_Utility this library is dependent on that utility. 
*/
#include <Bonezegei_Utility.h>
#include <Bonezegei_XPT2046v2.h>



#define TS_CS 33
Bonezegei_XPT2046v2 ts(TS_CS);

Bonezegei_Input   *input;

void setup() {

  Serial.begin(115200);
  input   = &ts;
  
  input->begin();
  input->setRotation(1);

}

void loop() {
  if(input->getPress()){
    Serial.print("X: "); Serial.print(input->point.x);
    Serial.print(" Y: "); Serial.println(input->point.y);
  }
  delay(100);
}
