/*
  This Library is written for ILI9341 LCD
  Author: Bonezegei (Jofel Batutay)
  Date: July 2023 
*/
#ifndef _FONT_H_
#define _FONT_H_
#include "fonts/Arial8.h"
#include "fonts/Arial10.h"
#include "fonts/Arial12.h"
#include "fonts/Arial14.h"
#include "fonts/Arial16.h"

#include "fonts/Arial8Bold.h"
#include "fonts/Arial10Bold.h"
#include "fonts/Arial12Bold.h"

typedef enum {
  FONT_ARIAL_8=0,
  FONT_ARIAL_10,
  FONT_ARIAL_12,
  FONT_ARIAL_14,
  FONT_ARIAL_16,
  FONT_ARIAL_8_BOLD,
  FONT_ARIAL_10_BOLD,
  FONT_ARIAL_12_BOLD,
} FONT_TYPE;


class FONT {
public:
  FONT() {}
  char *fnt;
  int descriptor[95][3];
};




#endif
