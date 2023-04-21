// Libraries------------------------------------------
#include <M5StickCPlus.h>
#include <AdafruitIO_WiFi.h>
#include <Adafruit_NeoPixel.h>
// Header files---------------------------------------
#include "Functs.h"
#include "M5StickWiFi.h"
// Connecting to Asafruit feeds-----------------------
AdafruitIO_WiFi io("ADAFRUIT USERNAME", "ADAFRUIT KEY", "NETWORK NAME", "NETWORK PASSWORD");
AdafruitIO_Feed *INOUT = io.feed("ChickenFeed");
// Variables that are used in code--------------------
int DisStart;

void setup() {
  // Setting up M5stick----------------------
  M5.begin();
  // Setting up Neo Pixel--------------------
  NeoPixel.begin();
  NeoPixel.setBrightness(100);
  // Connectiong to Adafruit-----------------
  M5.Lcd.println("Connecting to AdafruitIO");
  io.connect();
  delay(1000);
  INOUT->onMessage(handleMessage);
  
  while (io.status() < AIO_CONNECTED) {
    M5.Lcd.print(".");
    delay(500);
  }
  INOUT->get();
  // conect to wifi to Fetch API information------------------
  connectToWifi("NETWORK NAME", "NETWORK PASSWORD");
  // These are functions defined within the header file Functs.h
  getTimeData(); 
  TimeFetch();
  SetupTime();
  getDayData();
  getSun();
  // Compleat setup-------------------------
  M5.Lcd.fillScreen(BLACK);
  // Set variabls for time delay insted of using delay function
  TimeStart = millis();
  DisStart = millis();
}

// Futher variables that need to be defined after setup----------
String Day = String(RTC_DateStruct.Date); // change in Day
int DisNum = 0; // Display naummber for M5 Lcd
String TimeAsString;

void loop() {
  // change between displays--------------
  // All functions in this section are defined in the heder file Functs.h
  if(DisNum == 0) {
    DisTime(); // Function that displays the current time and date 
  }

  else if(DisNum == 1) {
    SunRiseSet(); // Function that displays Sun rise and set
  }

  else if(DisNum == 2) {
    LengthOfDay(); // Function that displays the Day length 
  }
  // Change desplay when button A is pressed-------------------------
  if(digitalRead(M5_BUTTON_HOME) == LOW && (millis()-DisStart)>250){
  
      if(DisNum == 0){
        DisNum = 1;
      }
      else if(DisNum == 1){
        DisNum = 2;
      }
      else if(DisNum == 2){
        DisNum = 0;
      }
      // reset time dilay
      DisStart = millis();
  }
  TimeAsString = (String(RTC_TimeStruct.Hours)+String(RTC_TimeStruct.Minutes)+String(RTC_TimeStruct.Seconds));
  // connect two divices at night----------------
  //SunSet + 120000 converts
  if(INTSTR(SunRise) >= TimeAsString.toInt() || INTSTR(SunSet)+120000 <= TimeAsString.toInt() ){
  // Check for updata on AdaFruit feed-----------
  io.run();
  // the processes that happen when the feed changes are found in the handleMessage function
  }
  else{
    // During daylight hours, divices are disconnected.
    NeoPixel.clear();
    NeoPixel.show();
    FirstTrigger = 0;
  }
  // Update sunrise and sunset for a new day------------
  if(INTSTR(SunRise) >= TimeAsString.toInt()){
    if (String(RTC_DateStruct.Date) != Day){
      getDayData();
      getSun();
      Day = String(RTC_DateStruct.Date);
    }
  }
}