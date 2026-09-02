/*
  This Library is written for ILI9341 LCD
  Author: Bonezegei (Jofel Batutay)
  Date: July 2023 
*/

#ifndef _RECT_H_
#define _RECT_H_

#include "Point.h"


//Simple Rectangle Class For UI
class Rect {
public:
  Rect() {
    x1 = 0;
    x2 = 0;
    y1 = 0;
    y2 = 0;
  }

  Rect(int xa, int ya, int xb, int yb) {
    x1 = xa;
    y1 = ya;
    x2 = xb;
    y2 = yb;
	p1 = Point(xa,ya);
	p2 = Point(xb,yb);
  }

  char isPointInside(int x, int y) {
    if ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2)) {
      return 1;
    } else {
      return 0;
    }
  }

  char isPointInside(Point p) {
    if ((p.x >= x1) && (p.x <= x2) && (p.y >= y1) && (p.y <= y2)) {
      return 1;
    } else {
      return 0;
    }
  }

  Rect operator+(const Point& pos) const{
		Rect ret(*this);
		ret.x1+=pos.x;
		ret.y1+=pos.y;
		ret.x2+=pos.x;
		ret.y2+=pos.y;
		return ret;
  }
	
  int x1, y1, x2, y2;
  Point p1;
  Point p2;
};



#endif
