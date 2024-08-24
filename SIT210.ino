/*
  WriteMultipleFields
  
  Description: Writes values to fields 1,2,3,4 and status in a single ThingSpeak update every 20 seconds.
  
  Hardware: Arduino MKR1000
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires WiFi101 library version 0.15.3 or newer.
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/

#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h" //include thingspeak header file after other header files and custom macros
#include "DHT.h"

#define DHTPIN 2 
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);
char ssid[] = SECRET_SSID;   //  network SSID (name) 
char pass[] = SECRET_PASS;   //  network password
int keyIndex = 0;            // network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID; 
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;



void setup() {
  Serial.begin(115200);      // Initialize serial 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  dht.begin();
  ThingSpeak.begin(client);  // Initialize ThingSpeak 
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  delay(60000); //60 second delay
   // Sensor readings can be up to 2 seconds 'old' 
  float h = dht.readHumidity();
  // to read the temperature as celsius
  float t = dht.readTemperature();
   
    if (isnan(h) || isnan(t))  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  ThingSpeak.setField(1, t); //t= temperature 
  ThingSpeak.setField(2, h);//h= humidity

String myStatus = "Temperature and Humidity data updated";  //message to indicate that data is being updated

  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }

  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}

