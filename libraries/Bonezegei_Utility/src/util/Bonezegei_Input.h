/*
  This Library is written for ILI9341 LCD
  Author: Bonezegei (Jofel Batutay)
  Date: July 2023 
*/

#ifndef _BONEZEGEI_INPUT_H_
#define _BONEZEGEI_INPUT_H_

#include "Point.h"
#include "Rect.h"

class Bonezegei_Input {
public:

  Bonezegei_Input(){
    x = 0; 
    y = 0; 
    z = 0; 
    zT = 500;
    rotation = 0;
    point = Point(0,0);
    width = 320;
    height = 240;
    
    xmul =  0.092219;
    ymul =  0.06857;
    xOff = -20;
    yOff = -20;
    }
  
  virtual void begin(){}

protected:

  virtual uint8_t getInput(){return 0;}

public:

  void setRotation(uint8_t r){
    rotation=r;
  }

  Point getPoint(){
    if(getInput()){
        calculate();
		if(_x<=0)_x=0;
		if(_y<=0)_y=0;
        point.x=_x;
        point.y=_y;
    return point;
    }
    return Point(0,0);
  }

  int8_t getPress(){
    if(getInput()){
        calculate();
  		  if(_x<=0)_x=0;
		    if(_y<=0)_y=0;
        point.x=_x;
        point.y=_y;
        return 1;
    }
    return 0;
  }

  void calculate(){

    switch (rotation){
    case 0:
        _x = (xmul*x) + xOff;
        _y = (ymul*y) + yOff;
    break;
    case 1:
        _x = width -( (xmul*x) + xOff);
        _y = height - ( (ymul*y) + yOff);       
    break;

    default:
        break;
    }
  }


  int x;
  int y;
  int z;

  int _x;
  int _y;

  Point point;
  
  float xmul;
  float ymul;

/*   int16_t xHigh;
  int16_t xLow;
  int16_t yHigh;
  int16_t yLow; */
  int16_t xOff;
  int16_t yOff; 

  int16_t width;
  int16_t height;


  int8_t rotation;

  uint16_t zT; // Z threshold for pressure
};


#endif
