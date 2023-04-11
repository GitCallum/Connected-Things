//=============================================================
// File:    M5StickCPlus_WiFi.h
// 
// Author:  J. Hathway
//
// Description:
//     - Function for connecting M5StickC Plus to WiFI Network.
//     - Requires nextwork name and password.
//     - Prints out message on LCD when successful/failed.
//=============================================================
#include <M5StickCPlus.h>
#include <WiFi.h>

WiFiClient client;

/// Function for connecting M5 board to WiFi
/// @param myNetwork (const char*) name of network
/// @param myPassword (const char*) network password
/// @param timeout (int) optional - time spent attempting to access network
void connectToWifi(const char* myNetwork, const char* myPassword, int timeout = 20000) {
  
  M5.Lcd.printf("Connecting");

  // initialise - WIFI_STA = Station Mode, WIFI_AP = Access Point Mode 
  WiFi.mode(WIFI_STA);
  WiFi.begin(myNetwork, myPassword);

  // start timer
  unsigned long startAttemptTime = millis();

  // while not connected to WiFi AND before timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    M5.Lcd.printf(".");
    delay(400);
  }

  // if failed, print LCD message
  if (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.println(" Failed!");
  }
  // if successful, print LCD message
  else {
    M5.Lcd.println("\nConnected to:");
    M5.Lcd.println(myNetwork);
    delay(2000);
  }
}