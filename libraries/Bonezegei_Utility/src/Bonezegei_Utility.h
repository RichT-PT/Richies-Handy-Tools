/*
  This Library is written for All bonezgei Libraries
  Author: Bonezegei (Jofel Batutay)
  Date: July 2023 
  
  This Library contains several useful header files for bonezegei (Rectangles Class, Point Class)
*/
#ifndef _BONEZEGEI_UTILITY_H_
#define _BONEZEGEI_UTILITY_H_

#include <Arduino.h>

#include "util/Point.h"
#include "util/Rect.h"
#include "util/Bonezegei_Display.h"
#include "util/Bonezegei_Input.h"

#include "bitmaps/default.h"
#include "Font.h"

const Bitmap BitmapBonezegei = Bitmap(bonezegeismallWidthPages, bonezegeismallHeightPixels, (char*)bonezegeismallBitmaps);
const Bitmap BitmapClose     = Bitmap(closeWidthPages, closeHeightPixels, (char*)closeBitmaps);

class Bonezegei_Utility {
public:
};


#endif
