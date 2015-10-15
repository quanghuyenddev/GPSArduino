#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>
#define debug

// pin connect to CK of SD Card Board
#define SD_ChipSelectPin 10

TMRpcm tmrpcm;
Sd2Card card;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
File songsFile;
char nameBuffer[10];
int pos = 0;

int pauseButton = 2;
boolean isStopped = false;

int nextButton = 3;

int loopButton = 4;
boolean isLoop = false;

void setup() {
  
  Serial.begin(9600);
  pinMode(pauseButton, INPUT);
  pinMode(nextButton, INPUT);
  pinMode(loopButton, INPUT);
  
  // create LCD 20 chars x 4 lines, turn on backlight
  lcd.begin(20, 4);
  lcd.backlight();

  // set pin connect to speaker
  tmrpcm.speakerPin = 9;

  // check if SD Card connect succeed
  if (!SD.begin(SD_ChipSelectPin)) {
    lcd.setCursor(0, 0);
    lcd.print("Initialization");
    lcd.setCursor(0, 1);
    lcd.print("SD Card");
    lcd.setCursor(0, 2);
    lcd.print("Failed");
    return;
  }
  else {
    lcd.setCursor(0, 0);
    songsFile = SD.open("SONGS.txt", FILE_READ);
    
    if (songsFile) {
      while (songsFile.available()) {
        String songName = songsFile.readStringUntil('\n');
        int str_len = songName.length();
        char songNameChars[str_len];
        songName.toCharArray(songNameChars, str_len);
        
        lcd.setCursor(0, 0);
        lcd.print(songNameChars);
        Serial.println(songNameChars);
        tmrpcm.play(songNameChars);
        
        lcd.setCursor(0, 1);
        lcd.print("Playing");
        lcd.setCursor(0, 2);
        lcd.print("No loop");
        
        while (tmrpcm.isPlaying()) {
          int pauseStatus = digitalRead(pauseButton);
          if (pauseStatus) {
            isStopped = !isStopped;
            lcd.setCursor(0, 1);
            if (!isStopped) {
              lcd.print("Playing");
            }
            else {
              lcd.print("Paused!");
            }
            tmrpcm.pause();  
            delay(1000);
          }
          
          int nextStatus = digitalRead(nextButton);
          if (nextStatus) {
            tmrpcm.stopPlayback();
            tmrpcm.loop(false);
            delay(1000);
          }

          int loopStatus = digitalRead(loopButton);
          if (loopStatus) {
            isLoop = !isLoop;
            tmrpcm.loop(isLoop);
            lcd.setCursor(0, 2);
            if (isLoop) {
              lcd.print("Loop!!!");
            }
            else {
              lcd.print("No loop");
            }
            delay(1000);
          }
                    
        }
        lcd.clear();

      }

    }
    lcd.clear();
    lcd.setCursor(7, 0);
    lcd.print("FINISH!");
  }
}



void loop() {
  
}
