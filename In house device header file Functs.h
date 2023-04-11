// Libraries----------------------------------
#include <M5StickCPlus.h>
#include <AdafruitIO_WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// Neo pixel setup----------------------------
#define NEOPIN G26 // output pin
#define NUM_PIXELS  8 // number of pixels
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);
// Structure of Time and date-----------------
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
// API url's----------------------------------
String TimeURL = "http://worldtimeapi.org/api/timezone/Europe/London";
String DayURL = "https://api.sunrise-sunset.org/json?lat=55.9533&lng=3.1883&date=today";
// Store data feched from API's----------------------
StaticJsonDocument<1500> TimeDoc;
StaticJsonDocument<1500> Daydoc;
// Functions to make printing on Lcd easier----------
#define PRINT(x) M5.Lcd.print(x)
#define PRINT_LN(x) M5.Lcd.println(x)
// variables for  Display number and time dilay------
int Display = 0;
int TimeStart;
// variables for handeling sun data------------------
String ComplexTime;
String SunRise;
String SunSet;
String DayLength;

// turn the data retrieved from sunrise-sunset API into a an integer
int INTSTR(String empt){
    int latiLength = empt.indexOf(":"); // break down times into integer components
    empt.remove(latiLength, 1);
    latiLength = empt.indexOf(":");
    empt.remove(latiLength, 1);
    latiLength = empt.indexOf(" ");
    empt.remove(latiLength);
    return empt.toInt(); // convert to integer
}

// get sata about exact time
void TimeFetch(){
    const char* Complex = TimeDoc["datetime"];
    ComplexTime = String(Complex);
}
// get relevent time data about sunrise/sunset and day lenth
void getSun(){   
    JsonObject daily = Daydoc["results"];
    const char* Rise = daily["sunrise"];
    const char* Set = daily["sunset"];
    const char* Length = daily["day_length"];
    // Save data as strings
    SunRise = String(Rise); 
    SunSet = String(Set);
    DayLength = String(Length)+" ";
}

// Fetch data from worldtimeapi API
void getTimeData(){
        // set up HTTP Client
        HTTPClient client;
        // access URL
        client.begin(TimeURL.c_str());
        // send GET Request
        int httpCode = client.GET();
        // if request is successful...
        if (httpCode > 0){
            // store recieved data
            String payload = client.getString();
            PRINT_LN("Statuscode: " + String(httpCode));
            // check for error
            DeserializationError error = deserializeJson(TimeDoc, payload);
            if (error){
                PRINT("deserializeJson() failed: ");
                PRINT_LN(error.c_str());
                return;
            }
            else{
                PRINT_LN("deserializeJson() successful");

                delay(1000);
                M5.Lcd.fillScreen(BLACK);
                M5.Lcd.setCursor(0, 0);
            }
            // end HTTP call
            client.end();
        }
        else{
            // if request is NOT successful, print error message
            PRINT_LN("Error on HTTP request.");
        }
    }
    // Fetch data from sunrise-sunset API
    void getDayData(){
        // set up HTTP Client
        HTTPClient client;
        // access URL
        client.begin(DayURL.c_str());
        // send GET Request
        int httpCode = client.GET();
        // if request is successful...
        if (httpCode > 0){
            // store recieved data
            String payload = client.getString();
            PRINT_LN("Statuscode: " + String(httpCode));
            // check for error
            DeserializationError error = deserializeJson(Daydoc, payload);
            if (error){
                PRINT("deserializeJson() failed: ");
                PRINT_LN(error.c_str());
                return;
            }
            else{
                PRINT_LN("deserializeJson() successful");
                delay(1000);
                M5.Lcd.fillScreen(BLACK);
                M5.Lcd.setCursor(0, 0);
            }
            // end HTTP call
            client.end();
        }
        else{
            // if request is NOT successful, print error message
            PRINT_LN("Error on HTTP request.");
        }
    }
// Set the time on the M5 as the time retrieved from worldtimeapi API
void SetupTime(){
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Hours = ComplexTime.substring(11,13).toInt();
    TimeStruct.Minutes = ComplexTime.substring(14,16).toInt();
    TimeStruct.Seconds = ComplexTime.substring(17,19).toInt();
    M5.Rtc.SetTime(&TimeStruct);

    RTC_DateTypeDef DateStruct;
    DateStruct.Month = ComplexTime.substring(5,7).toInt();
    DateStruct.Date = ComplexTime.substring(8,10).toInt();
    DateStruct.Year = ComplexTime.substring(0,5).toInt();
    M5.Rtc.SetData(&DateStruct);
}
// Display 1, shows time and date------------------------------------
void DisTime(){
    if (millis() - TimeStart  > 1000){
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setTextSize(2);
        M5.Rtc.GetTime(&RTC_TimeStruct);
        M5.Rtc.GetData(&RTC_DateStruct);
        M5.Lcd.setCursor(20, 45, 4);
        M5.Lcd.printf("%02d:%02d:%02d\n", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
        M5.Lcd.setCursor(42, 10, 2);
        M5.Lcd.setTextSize(2);
        M5.Lcd.printf("%04d-%02d-%02d\n", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
        TimeStart = millis();
    }
}
// Display 2 shows sun rise and sun set
void SunRiseSet(){
    //if statment acts as delay without halting everything
    if (millis() - TimeStart  > 1000){
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(70, 10, 1);
    M5.Lcd.print("Sun Rise");
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 55, 4);
    M5.Lcd.print("------------------------------");
    M5.Lcd.setCursor(55, 35, 4);
    M5.Lcd.print(SunRise);
    M5.Lcd.setCursor(75, 75, 1);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("Sun Set");
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(55, 100, 4);
    M5.Lcd.print(SunSet);
    // reset the delay
    TimeStart = millis();
    }


}
// Display 3 shows day length----------
void LengthOfDay(){
    //if statment acts as delay without halting everything
    if (millis() - TimeStart  > 1000){
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(60, 30, 1);
    M5.Lcd.println("Day Length");
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(70, 55, 4);
    M5.Lcd.println(DayLength);
    // reset the delay
    TimeStart = millis();
    }
}
// variables needed to handle the Adafruit feed
String ReceivedSTR;
int FirstTrigger = 0;
// what happens when feed changes--------------------
void handleMessage(AdafruitIO_Data *data) {

    M5.Lcd.fillScreen(BLACK);
    ReceivedSTR = String(data->value()); // retrieve data
    // first time sesnor gets triggered, Neo Pixel will light up-----------------
    if (ReceivedSTR == "1" && FirstTrigger == 0){
        for (int pixel = 0; pixel < NUM_PIXELS; pixel++){
            NeoPixel.setPixelColor(pixel, NeoPixel.Color(240, 110, 30));
            NeoPixel.show();
            M5.Lcd.fillScreen(BLACK);
            delay(250);
            M5.Lcd.setTextSize(2);
            M5.Lcd.setCursor(10,10,2);
            PRINT_LN("Chicken In");
            delay(250);
        }
    // Neo pixel stays ON
    FirstTrigger = 1;
    }
    // if sensor is LOW, Neo Pixel will flash fed to indicate the Chicken has been desturbed---
    if(ReceivedSTR == "0" && FirstTrigger == 1){
        for(int rep = 0; rep < 4; rep++){
            for (int pixel = 0; pixel < NUM_PIXELS; pixel++){
                NeoPixel.setPixelColor(pixel, NeoPixel.Color(228, 10, 14));
            }
            NeoPixel.show();
            M5.Lcd.setTextSize(2);
            M5.Lcd.setCursor(10,10,2);
            PRINT_LN("DANGER");
            delay(1000);
        for (int pixel = 0; pixel < NUM_PIXELS; pixel++){
            NeoPixel.setPixelColor(pixel, NeoPixel.Color(240, 110, 30));
        }
        NeoPixel.show();
        M5.Lcd.fillScreen(BLACK);
        delay(1000);
        }
        for (int pixel = 0; pixel < NUM_PIXELS; pixel++){
            NeoPixel.setPixelColor(pixel, NeoPixel.Color(228, 10, 14));
        }
        NeoPixel.show(); 
    }
    // if sensor is then triggered again light will go back to normal----
    if(ReceivedSTR == "1" && FirstTrigger == 1){
        for (int pixel = 0; pixel < NUM_PIXELS; pixel++){
            NeoPixel.setPixelColor(pixel, NeoPixel.Color(240, 110, 30));
        }
        NeoPixel.show();
    }
}