#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "words5.h"
#include "words6.h"
#include "words7.h"
#include "words8.h"
#include "nsfw5.h"
#include "nsfw6.h"
#include "nsfw7.h"
#include "nsfw8.h"

#define LEDPIN PB1
#define BUTTON PB5
#define MIN_WORD_LENGTH 5
#define MAX_WORD_LENGTH 8

const int words5_count = words5_txt_len / 5;
const int words6_count = words6_txt_len / 6;
const int words7_count = words7_txt_len / 7;
const int words8_count = words8_txt_len / 8;
const int nsfw5_count = nsfw5_txt_len / 5;
const int nsfw6_count = nsfw6_txt_len / 6;
const int nsfw7_count = nsfw7_txt_len / 7;
const int nsfw8_count = nsfw8_txt_len / 8;

bool buttonPressed = false;
unsigned long pressStart;
bool randomSeeded = false;
int nsfwLevel = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  pinMode(BUTTON, INPUT_PULLUP);
  lcd.begin();
  lcd.print("     SPUGE      ");
  lcd.setCursor(0,1);
  lcd.print("- paina nappia -");
}


void loop() {
  if(digitalRead(BUTTON) == LOW) {
    if(! buttonPressed) {
      onButtonPressed();
    }
    buttonPressed = true;
  } else {
    if(buttonPressed) {
      onButtonReleased();
    }
    buttonPressed = false;
  }
}

void onButtonPressed() {
  pressStart = micros();
}

void onButtonReleased() {
  unsigned long pressedTime = micros() - pressStart;
  if(! randomSeeded) {
    randomSeed(pressedTime);
    randomSeeded = true;
  }
  if(pressedTime > 40000) {
    if(pressedTime < 1000000) {
      digitalWrite(LEDPIN, HIGH);
      renderRandomWords();
      digitalWrite(LEDPIN, LOW); 
    } else {
      nsfwLevel = min((pressedTime - 1000000) / 200000, 10);
      lcd.clear();
      lcd.print(" PIRITORI-MOODI ");
      lcd.setCursor(0,1);
      if(nsfwLevel < 10) {
        lcd.print("  -- taso ");
        lcd.print(nsfwLevel);
        lcd.print(" --  ");
      } else {
        lcd.print(" -- ALL IN!! -- ");
      }
    }
  } 
}

void renderRandomWords() {
  lcd.clear();
  lcd.setCursor(0,0);
  int wordLength = getNextWordLength(0);
  renderWord(wordLength);
  lcd.print(" ");
  
  wordLength = getNextWordLength(wordLength);
  renderWord(wordLength);
  
  lcd.setCursor(0,1);
  wordLength = getNextWordLength(0);
  renderWord(wordLength);

  lcd.print(" ");
  wordLength = getNextWordLength(wordLength);
  renderWord(wordLength);
}

int getNextWordLength(int previousLength) {
  int nextLength = 100;
  while(previousLength + nextLength >= 16) {
    nextLength = random(MIN_WORD_LENGTH, MAX_WORD_LENGTH + 1);
  }
  return nextLength;
}

void renderWord(int charCount) {
  unsigned const char* source;
  int sourceWordCount;
  
  if(charCount == 5) {
    if(random(0, 15) >= nsfwLevel) {
      source = words5_txt;
      sourceWordCount = words5_count - 1;  
    } else {
      source = nsfw5_txt;
      sourceWordCount = nsfw5_count - 1;        
    }
  } else if(charCount == 6) {
    if(random(0, 15) >= nsfwLevel) {
      source = words6_txt;
      sourceWordCount = words6_count - 1;
    } else {
      source = nsfw6_txt;
      sourceWordCount = nsfw6_count - 1;              
    }
  } else if(charCount == 7) {
    if(random(0, 15) >= nsfwLevel) {
      source = words7_txt;
      sourceWordCount = words7_count - 1;
    } else {
      source = nsfw7_txt;
      sourceWordCount = nsfw7_count - 1;              
    }
  } else {
    if(random(0, 15) >= nsfwLevel) {
      source = words8_txt;
      sourceWordCount = words8_count - 1;
    } else {
      source = nsfw8_txt;
      sourceWordCount = nsfw8_count - 1;              
    }
  }
    
  renderWord(source, random(0, sourceWordCount) * charCount, charCount);
}

void renderWord(unsigned const char *src, int index, int length) {
  char buffer[length + 1];
  strncpy((char*)&buffer, (const char*)src + index, length);
  buffer[length] = 0;
  lcd.print(buffer);
}

