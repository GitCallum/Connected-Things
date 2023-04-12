// Libraries----------------------------------
#include <M5StickCPlus.h>
#include <AdafruitIO_WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
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