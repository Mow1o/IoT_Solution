#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#include "mbed.h"
#include "glcdfont.h"
#include "Stream.h"   // testing tk

#define BLACK 0
#define WHITE 1
#ifndef _BV
#define _BV(bit) (1<<(bit))
#endif
#define swap(a, b) { int16_t t = a; a = b; b = t; }

/** Adafruit_GFX class. This is the Adafruit_GFX class.\n drawPixel(int16_t x,int16_t y,uint16_t color) needing implementation in derived implementation class
*/
class Adafruit_GFX : public Stream
{
public:
    Adafruit_GFX(int16_t w, int16_t h)
        : _rawWidth(w),_rawHeight(h)
        ,_width(w),_height(h)
        ,cursor_x(0),cursor_y(0)
        ,textcolor(WHITE),textbgcolor(BLACK)
        ,textsize(1)
        ,rotation(0)
        ,wrap(true)
    {};

    /** @fn void Adafruit_GFX::drawPixel(int16_t x, int16_t y, uint16_t color) = 0
    *   @bref This MUST be defined by the subclass!
    *   @param x x
    *   @param y y
    *   @param color 16bit color
    */
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
// These are overridden for the Stream subclass
    virtual int _putc(int value) {
        return writeChar(value);
    };
    virtual int _getc() {
        return -1;
    };
    /** @fn void Adafruit_GFX::invertDisplay(bool i)
    *   @bref Do nothing, must be subclassed if supported
    *   @param i invert
    */
    virtual void invertDisplay(bool i);

// These MAY be overridden by the subclass to provide device-specific
// optimized code.  Otherwise 'generic' versions are used.
    virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    virtual void fillScreen(uint16_t color);

//These exist only with Adafruit_GFX (no subclass overrides)
    /** @fn void Adafruit_GFX::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
    *   @bref draw a circle outline from the coordinates of the center.
    *   @param x0 x position
    *   @param y0 y position
    *   @param r Radius of the circle
    *   @param color 16bit color
    */
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
    /** @fn void Adafruit_GFX::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
    *   @bref draw a circle from the coordinates of the center.
    *   @param x0 x position
    *   @param y0 y position
    *   @param r Radius of the circle
    *   @param color 16bit color
    */
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);

    /** @fn void Adafruit_GFX::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
    *   @bref draw a triangle outline from the vertex with color.
    *   @param x0 first vertex x
    *   @param y0 first vertex y
    *   @param x1 second vertex x
    *   @param y1 second vertex y
    *   @param x2 third vertex x
    *   @param y2 third vertex y
    *   @param color 16bit color
    */
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    /** @fn void Adafruit_GFX::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
    *   @bref draw a triangle from the vertex with color.
    *   @param x0 first vertex x
    *   @param y0 first vertex y
    *   @param x1 second vertex x
    *   @param y1 second vertex y
    *   @param x2 third vertex x
    *   @param y2 third vertex y
    *   @param color 16bit color
    */
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

    /** @fn void Adafruit_GFX::drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
    *   @bref draw a round rectangle outline with color.
    *   @param x0 first vertex x
    *   @param y0 first vertex y
    *   @param w width
    *   @param h height
    *   @param radius radius
    *   @param color 16bit color
    */
    void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
    /** @fn void Adafruit_GFX::drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
    *   @bref draw a round rectangle with color.
    *   @param x0 first vertex x
    *   @param y0 first vertex y
    *   @param w width
    *   @param h height
    *   @param radius radius
    *   @param color 16bit color
    */
    void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);

    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

    void setCursor(int16_t x, int16_t y) {
        cursor_x = x;
        cursor_y = y;
    };

    void setTextSize(uint8_t s) {
        textsize = (s > 0) ? s : 1;
    };

    void setTextColor(uint16_t c) {
        textcolor = c;
        textbgcolor = c;
    }
    void setTextColor(uint16_t c, uint16_t b) {
        textcolor = c;
        textbgcolor = b;
    };
    void setTextWrap(bool w) {
        wrap = w;
    };

    virtual size_t writeChar(uint8_t);

    int16_t height(void) {
        return _height;
    };

    int16_t width(void) {
        return _width;
    };
    void setRotation(uint8_t r) {
        rotation = r;
    }

    uint8_t getRotation(void) {
        rotation%=4;
        return rotation;
    };

protected:
    int16_t _rawWidth, _rawHeight; // Display w/h as modified by current rotation
    int16_t _width, _height; // Display w/h as modified by current rotation
    int16_t cursor_x, cursor_y;
    uint16_t    textcolor, textbgcolor;
    uint8_t textsize,rotation;
    bool    wrap; // If set, 'wrap' text at right edge of display
};

#endif // _ADAFRUIT_GFX_H
