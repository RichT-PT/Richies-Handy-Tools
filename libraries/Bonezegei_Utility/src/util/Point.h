/*
  This Library is written for ILI9341 LCD
  Author: Bonezegei (Jofel Batutay)
  Date: July 2023 
*/

#ifndef _POINT_H_
#define _POINT_H_


//Simple Point Class For UI
class Point {
public:

  Point() {
    x = 0;
    y = 0;
  }

  Point(int _x, int _y) {
    x = _x;
    y = _y;
  }

  int x, y;
};


#endif