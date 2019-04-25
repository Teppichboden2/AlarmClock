#include "LCD.h"

LCD* LCD::instance = nullptr;

void LCD::init(uint8_t rs, uint8_t enable, uint8_t back, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t columns, uint8_t rows) {
  if(instance == nullptr) {
    instance = new LCD(rs, enable, back, d0, d1, d2, d3);
    instance->begin(columns,rows);
  }
}

LCD* LCD::getInstance() {
  return instance;
}

LCD::LCD(uint8_t rs, uint8_t enable, uint8_t back, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
  : LiquidCrystal(rs, enable, d0, d1, d2, d3), backlight(back) {
  pinMode(backlight, OUTPUT);
  digitalWrite(backlight, HIGH);
  this->createChars();
  Serial.println("LCD created");
}

void LCD::toggleBacklight() const {
  digitalWrite(backlight, !digitalRead(backlight));
}

void LCD::createChars() {
  uint8_t bell[] = {
    B00100,
    B01110,
    B01110,
    B01110,
    B11111,
    B00000,
    B00100,
    B00000
  };

  uint8_t arrowup[] = {
    B00100,
    B01110,
    B11111,
    B00100,
    B00100,
    B00100,
    B00100,
    B00000
  };

  uint8_t arrowdown[] = {
    B00000,
    B00100,
    B00100,
    B00100,
    B00100,
    B11111,
    B01110,
    B00100
  };

  uint8_t arrowleft[] = {
    B00000,
    B00000,
    B00100,
    B01100,
    B11111,
    B01100,
    B00100,
    B00000
  };

  uint8_t arrowright[] = {
    B00000,
    B00000,
    B00100,
    B00110,
    B11111,
    B00110,
    B00100,
    B00000
  };

  uint8_t degree[] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000
  };

  this->createChar(BELL, bell);
  this->createChar(ARRU, arrowup);
  this->createChar(ARRD, arrowdown);
  this->createChar(ARRL, arrowleft);
  this->createChar(ARRR, arrowright);
  this->createChar(DEGR, degree);
}
