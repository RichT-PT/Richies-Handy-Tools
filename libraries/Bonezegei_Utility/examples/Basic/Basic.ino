/*
  Author: Jofel Batutay
  Date: July 2023

  Usage Sample

  Simple Rectangle Class
  and Point Class
*/

#include "Bonezegei_Utility.h"


void setup() {
  Serial.begin(115200);

  Rect rect(1, 1, 100, 100);
  Point point1(50, 50);
  Point point2(101, 101);

  Serial.println("Rectangle:");
  Serial.print("X1:");
  Serial.print(rect.x1);
  Serial.print("  Y1:");
  Serial.print(rect.y1);
  Serial.print("  X2:");
  Serial.print(rect.x2);
  Serial.print("  Y2:");
  Serial.println(rect.y2);

  Serial.println("is Point inside Rectangle:");
  Serial.print("PX:");
  Serial.print(point1.x);
  Serial.print("  PY:");
  Serial.print(point1.y);
  Serial.print(" :");
  if (rect.isPointInside(point1)) {
    Serial.println("true");
  }
  else{
    Serial.println("false");
  }


  Serial.print("PX:");
  Serial.print(point2.x);
  Serial.print("  PY:");
  Serial.print(point2.y);
  Serial.print(" :");
  if (rect.isPointInside(point2)) {
    Serial.println("true");
  }
  else{
    Serial.println("false");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
}
