/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "string.h"

#define PIN_COUNTER 34

unsigned long period_get = 30000;
unsigned long period_post = 3000;
unsigned long last_time_get = 0;
unsigned long last_time_post = 0;


const char* ssid = "3bb";
const char* password = "0844025188";

void setup() {
    pinMode( PIN_COUNTER , INPUT );
    attachInterrupt(digitalPinToInterrupt(PIN_COUNTER), countUp, FALLING );
    Serial.begin(115200);

    Serial.println();
    Serial.println();
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

String msg;
volatile uint32_t count;
int8_t flag = 0;

void loop() {
    if(WiFi.status()== WL_CONNECTED )
    {
        char buff[100];
        if( millis() - last_time_get > period_get) 
        {
            last_time_get = millis();
            sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/mc.php?id_mc=02-01&count=%d&qty=55" , count);
            Serial.println( buff );
            msg = httpPOSTRequest(buff,"");         
            Serial.println( msg );      
        }

        if ( flag ) 
        {
            Serial.println( count ); 
            flag = 0; 
        }
/*
        if( millis() - last_time_post > period_post) 
        {
          last_time_post = millis();
          httpPOSTRequest("http://192.168.1.57:3000/","{ \"id\" : 1 }");
        }
*/

    }

      
}

void countUp ( void ) 
{
  count++;  
  flag = 1;
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

String httpPOSTRequest(const char* serverName , const char* msg) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
//  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Send HTTP POST request
  int httpResponseCode = http.POST(msg);
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void FunttestNodeJS( void ) 
{
             char buff[50];
             int num;
             num = random(3, 66);
            last_time_get = millis();
  //        msg = httpGETRequest("http://192.168.1.57:3000/");
            sprintf( buff , "http://192.168.1.57:3000/%d" , num );
            msg = httpGETRequest(buff);
  
            StaticJsonDocument<128> doc;
            DeserializationError error = deserializeJson(doc, msg);
            if (error) 
            {
//              Serial.print(F("deserializeJson() failed: "));
//              Serial.println(error.f_str());
                Serial.print(num);
                Serial.println(" : ID is not found !");
              return;
            }
            else
            {
                int id = doc["id"]; // 46
                const char* timestamp = doc["timestamp"]; // "2020-04-09T10:23:26.000Z"
                int value = doc["value"]; // 17
                sprintf( buff , "ID : %d TIMESTAMP : %s VALUE : %d" , id , timestamp , value );
                Serial.println( buff ) ;   
            }  
}
