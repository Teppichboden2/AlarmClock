#ifndef LCD_H
#define LCD_H

#include <LiquidCrystal.h>
#include <Arduino.h>

const uint8_t ARRU = 1;
const uint8_t ARRD = 2; 
const uint8_t DEGR = 3;
const uint8_t BELL = 4;

class LCD : public  LiquidCrystal {  
public:
  void toggleBacklight() const;
  
  static void init(uint8_t rs, uint8_t enable, uint8_t back, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t columns, uint8_t rows);
  static LCD* getInstance();

private:
  static LCD* instance;

  uint8_t backlight;
  void createChars();
  LCD(uint8_t rs, uint8_t enable, uint8_t back, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3); 
  ~LCD() {}

  LCD(LCD&) = delete;
  LCD(LCD&&) = delete;
};

#endif
