/*
  This Library is written for ILI9341 LCD
  Author: Bonezegei (Jofel Batutay)
  Date: July 2023 
*/

#ifndef _BONEZEGEI_DISPLAY_H_
#define _BONEZEGEI_DISPLAY_H_

#include "Point.h"
#include "Rect.h"
#include "Bitmap.h"
#include "../Font.h"


class Bonezegei_Display{
public:
	Bonezegei_Display(){
			
	}
	virtual void begin(){}
	virtual void setRotation(uint8_t rotation){}
	
	virtual void clear(uint32_t color){}
	virtual void setFont(FONT_TYPE ft){}

protected:
	// Raw Draw
	virtual void drawFilledRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color){}
    virtual void drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color){}
	virtual void drawRectangleStrip(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color){}
	virtual void drawRoundFilledRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint32_t color){}
	virtual void drawCircle(uint16_t x1, uint16_t y1, uint16_t r, uint32_t color){}
	virtual void drawBitmap(uint16_t x1, uint16_t y1, int xbytes, int yheight, const char bitmap[], uint32_t color){}
	virtual void drawText(int x, int y, const char *str, uint32_t color){}
	// Clipped Draw
	virtual void drawPixelClipped(uint16_t cx1, uint16_t cy1, uint16_t cx2, uint16_t cy2,uint16_t x, uint16_t y, uint32_t color){}
	virtual void drawTextClipped(uint16_t cx1, uint16_t cy1, uint16_t cx2, uint16_t cy2, int x, int y, const char *str, uint32_t color){}
	virtual void drawTextClippedNL(uint16_t cx1, uint16_t cy1, uint16_t cx2, uint16_t cy2, int x, int y, const char *str, uint32_t color){}
	virtual void drawBitmapClipped(uint16_t cx1, uint16_t cy1, uint16_t cx2, uint16_t cy2, uint16_t x1, uint16_t y1, int xbytes, int yheight, const char bitmap[], uint32_t color){}
	virtual void drawFilledRectangleClipped(uint16_t cx1, uint16_t cy1, uint16_t cx2, uint16_t cy2, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color){}
	virtual void drawRectangleClipped(uint16_t cx1, uint16_t cy1, uint16_t cx2, uint16_t cy2, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color){}
	
	virtual uint8_t getFontHeight();
        virtual uint16_t getStringWidth(char *str);

	virtual uint16_t getStringTotalHeight(uint16_t cx1, uint16_t cy1, uint16_t cx2, uint16_t cy2, int x, int y, const char *str);

public:
	
/// Text Draw
	// Draws a text from clipped rectangle 
	void drawText(Rect r, Point p,const char *str,uint32_t color){
		drawTextClipped(r.x1, r.y1, r.x2, r.y2, p.x+1, p.y+1, str, color);
	}
	
	// Draws a text from clipped rectangle 
	// NL_Word = new line if a word reaches the end of right margin
	void drawText(bool NL_Word, Rect r, Point p,const char *str,uint32_t color){
		if(NL_Word){
			drawTextClippedNL(r.x1, r.y1, r.x2, r.y2, p.x+1, p.y+1, str, color);}
		else{
			drawTextClipped(r.x1, r.y1, r.x2, r.y2, p.x+1, p.y+1, str, color);}
	}
	
	// Draws a text without clipping
	void drawText(Point p,const char *str,uint32_t color){
		drawText(p.x, p.y, str, color);
	}
	
	// Draws a text int the box
	void drawText(Rect r,const char *str,uint32_t color){
		drawTextClipped(r.x1, r.y1, r.x2, r.y2, r.x1+1, r.y1+1, str, color);
	}
	
	// Draws a text int the box
	void drawText(bool NL_Word,Rect r,const char *str,uint32_t color){
		if(NL_Word){
			drawTextClippedNL(r.x1, r.y1, r.x2, r.y2, r.x1+1, r.y1+1, str, color);}
		else{
			drawTextClipped(r.x1, r.y1, r.x2, r.y2, r.x1+1, r.y1+1, str, color);}
	}
	
	// Draws a text int the box
	void drawText(bool NL_Word,Rect c,Rect r,const char *str,uint32_t color){
		if(NL_Word){
			drawTextClippedNL(c.x1, c.y1, c.x2, c.y2, r.x1+1, r.y1+1, str, color);}
		else{
			drawTextClipped(c.x1, c.y1, c.x2, c.y2, r.x1+1, r.y1+1, str, color);}
	}
	
	
	
	// Draws a text int the center of the box one line text only
	void drawText(Rect r, bool center,const char *str,uint32_t color){
		if(center){
			int fontHeight = getFontHeight();
			int textWidth = getStringWidth((char*)str);
			int textX = r.x1 + (((r.x2 - r.x1) / 2) - (textWidth / 2));
			int textY = r.y1 + (((r.y2 - r.y1) / 2) - (fontHeight / 2)) + 1;
			drawText(textX, textY, str, color);
		}
		else{
			drawTextClipped(r.x1, r.y1, r.x2, r.y2, r.x1+1, r.y1+1, str, color);
		}
	}

	// Draws a clipped text int the center of the box one line text only
	void drawText(Rect c, Rect r, bool center,const char *str,uint32_t color){
		if(center){
			int fontHeight = getFontHeight();
			int textWidth = getStringWidth((char*)str);
			int textX = r.x1 + (((r.x2 - r.x1) / 2) - (textWidth / 2));
			int textY = r.y1 + (((r.y2 - r.y1) / 2) - (fontHeight / 2)) + 1;
			//drawText(textX, textY, str, color);
			drawTextClipped(c.x1, c.y1, c.x2, c.y2, textX, textY, str, color);
		}
		else{
			drawTextClipped(r.x1, r.y1, r.x2, r.y2, r.x1+1, r.y1+1, str, color);
		}
	}
	
	
///Draw Rectangle
	// Draw rectangle outline
	void drawRectangle(Rect r, uint32_t color){
		drawRectangle(r.x1, r.y1, r.x2, r.y2, color);
	}

	// Draw rectangle outline or filled (if true ) the retangle to be draaw will be filled
	void drawRectangle(bool fill,Rect r, uint32_t color){
		if(fill){
			drawFilledRectangle(r.x1, r.y1, r.x2, r.y2, color);}
		else{
			drawRectangle(r.x1, r.y1, r.x2, r.y2, color);}
	}

	// Draw rectangle outline
	void drawRectangle(Rect r, Rect c, uint32_t color){
		drawRectangleClipped(c.x1, c.y1, c.x2, c.y2, r.x1, r.y1, r.x2, r.y2, color);
	}	

	// Draw rectangle outline
	void drawRectangle(bool fill, Rect r, Rect c, uint32_t color){
		if(fill){
			drawFilledRectangleClipped(c.x1, c.y1, c.x2, c.y2, r.x1, r.y1, r.x2, r.y2, color);}
		else{
			drawRectangleClipped(c.x1, c.y1, c.x2, c.y2, r.x1, r.y1, r.x2, r.y2, color);
		}
	}	
	
	
///Draw Bitmap
	//draw bitmap from position
	void drawBitmap(Point p, Bitmap b,uint32_t color ){
		drawBitmap(p.x, p.y, b.xbytes, b.yheight, (const char*)b.data, color);
	}
	
	//draw bitmap on clip rectangle
	void drawBitmap(Rect r, Point p, Bitmap b, uint32_t color ){
		drawBitmapClipped(r.x1, r.y1, r.x2, r.y2, p.x, p.y,  b.xbytes, b.yheight, (const char*)b.data, color);
	}
	



	int getStringHeight(Rect r,const char *str){
		return getStringTotalHeight(r.x1, r.y1, r.x2, r.y2, r.x1+1, r.y1+1, str);
	}

	int xRun;
	int yRun;
	FONT font;
  
	Rect drawArea;
	char harwareName[16];
};


#endif
