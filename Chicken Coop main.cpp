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
// Servo settings-------------------------------------
const double PWM_Hz = 50;
const uint8_t PWM_level = 16;
const uint16_t MIN_degree0 = 1700;
const uint16_t MAX_degree180 = 5000;
int pwm_cnt = 26;
// sensor pin-----------------------------------------
#define SENSORPIN G0
// Neo pixel setup----------------------------
#define NEOPIN G36 // output pin
#define NUM_PIXELS  8 // number of pixels
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);


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

  while (io.status() < AIO_CONNECTED) {
    M5.Lcd.print(".");
    delay(500);
  }
  // Set servo-------------------------------
  pinMode(pwm_cnt, OUTPUT);
  ledcSetup((uint8_t)1, PWM_Hz, PWM_level);
  ledcAttachPin(pwm_cnt, 1);
  ledcWrite(1, MIN_degree0);
  // Sensor pin------------------------------
  pinMode(SENSORPIN, INPUT);
  digitalWrite(SENSORPIN, HIGH);
  M5.Lcd.fillScreen(BLACK);
  delay(1000);
  // conect to wifi to Fetch API information------------------
  connectToWifi("NETWORK NAME", "NETWORK PASSWORD");
  // These are functions defined within the header file Functs.h
  getTimeData(); 
  TimeFetch();
  SetupTime();
  getDayData();
  getSun();
}
// variables that look for change in sensor state------
int sensorState = 1, lastState = 1, reset = 0, Closed = 0, Extend_day = 0, Neo_ON = 0;
String TimeAsString;
String Day = String(RTC_DateStruct.Date); // change in Day


void loop() {
  // read sensor pin
  sensorState = digitalRead(SENSORPIN);
  // set current time as string variable
  TimeAsString = (String(RTC_TimeStruct.Hours)+String(RTC_TimeStruct.Minutes)+String(RTC_TimeStruct.Seconds));
  
  // how long Neo Pixel needs to be on to ensure 14 houres of daylight---------------
  if (INTSTR(DayLength) < 140000){
      Extend_day = 140000 - INTSTR(DayLength);
  }
  //If day lenth grater than 14 hours
  else{
    Extend_day = 0;
  }

  // if night time, look for chance in sensor state-----------------
  // Sunset + 120000 converts
  if(INTSTR(SunRise) >= TimeAsString.toInt() || INTSTR(SunSet)+120000 <= TimeAsString.toInt() ){
    // turn Neo Pixel ON
    if(Extend_day != 0 && TimeAsString.toInt() <= INTSTR(SunSet) + Extend_day && Neo_ON == 0){
      for (int pixel = 0; pixel < NUM_PIXELS; pixel++){
          NeoPixel.setPixelColor(pixel, NeoPixel.Color(240, 110, 30));
        }
        NeoPixel.show();
        Neo_ON == 1;
    }
    // Turn Neo Pixel off after Extend_day so that chickens get 14 hours of light
    if(TimeAsString.toInt() >= INTSTR(SunSet) + Extend_day && Neo_ON == 1){
      NeoPixel.clear();
      NeoPixel.show();
      Neo_ON == 0;
    }

    // if chicken in coop, close door
    if(sensorState != lastState){
      if(Closed == 0){
        ledcWrite(1, MAX_degree180);
        Closed = 1;
      }
      // if chicken is disturbed then send warning
      if(lastState == LOW){
        delay(1000);
        INOUT->save(0);
      } 
      // if chicken goes back to nest, cancel warning
      else{
        delay(1000);
        INOUT->save(1);
      } 
      // set the state to current state
      lastState = sensorState;
      // Indicator that sensor has changed
      reset = 1;
    }
  }
  // when daytime, open door and re set all indicators
  else if (reset = 1){
    ledcWrite(1, MIN_degree0);
    Closed == 0;
    reset = 0;
  }

  // Update sunrise and sunset for a new day-------------------------
  if(INTSTR(SunRise) >= TimeAsString.toInt()){
    if (String(RTC_DateStruct.Date) != Day){
      getDayData();
      getSun();
      Day = String(RTC_DateStruct.Date);
    }
  }
}