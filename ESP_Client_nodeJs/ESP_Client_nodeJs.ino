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

#include <Wire.h>
#include "dw_font.h"
#include "SSD1306.h"

#define PIN_COUNTER 34

unsigned long period_get = 1000;
unsigned long period_post = 3000;
unsigned long last_time_get = 0;
unsigned long last_time_post = 0;


const char* ssid = "3bb";
const char* password = "0844025188";

typedef struct employ { 
   char* id_staff ;
   char* name_first;
   char* name_last;
  uint8_t flag_err ;
  }employ_TYPE ; 

SSD1306   display(0x3c, 21, 22); //SDA - SCL
extern dw_font_info_t font_th_sarabun_new_regular14;
dw_font_t myfont;

void setup() {
    pinMode( PIN_COUNTER , INPUT );
    //attachInterrupt(digitalPinToInterrupt(PIN_COUNTER), countUp, FALLING );
    Serial.begin(115200);

    Serial.println();
    Serial.println();
    Serial.println();
    uint16_t width = 0;

  display.init();
  display.flipScreenVertically();

  dw_font_init(&myfont,
               128,
               64,
               draw_pixel,
               clear_pixel);

  dw_font_setfont(&myfont, &font_th_sarabun_new_regular14);

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

volatile uint32_t count;
int8_t flag = 0;
char buff[300];
String msg;


//employ_TYPE * dst = ( employ_TYPE *)malloc( sizeof( employ_TYPE ) + 1 ) ;
static employ_TYPE  dst;

void loop() {
    if(WiFi.status()== WL_CONNECTED )
    {

        while ( Serial.available() ) {
             msg = Serial.readString();  
             
 
             last_time_get = millis();
//             queryByID_GetMethod( ( const char * )msg.c_str() , &dst);

            if(  queryByID_GetMethod( ( const char * )msg.c_str() , &dst) == 0 ){
              /****/
              //             queryByID_GetMethod("0014084630" , &dst);
             sprintf( buff , "ID : %s TIMESTAMP : %s VALUE : %s" , dst.id_staff , dst.name_first , dst.name_last );
             Serial.println( buff );

//             memset( buff , 0 , 100 );

              display.resetDisplay();
//
              dw_font_goto(&myfont, 10, 10);
              sprintf( buff , "ID : %s" , dst.id_staff);
              Serial.println( buff );
              dw_font_print(&myfont, buff);
              display.display();
    

              dw_font_goto(&myfont, 10, 28);
              sprintf( buff , "FISRT : %s" ,dst.name_first);
              Serial.println(  buff );
              dw_font_print(&myfont, buff);
              display.display();

              dw_font_goto(&myfont, 10, 44);
              sprintf( buff , "LAST : %s" ,  dst.name_last);
              Serial.println( buff );
              dw_font_print(&myfont,buff);
              display.display();
                  /****/
              }
              else
              {
                
              }
              
             
          }
        
//        if( millis() - last_time_get > period_get) 
//        {
//              char buff[100];
//              last_time_get = millis();
//              queryByID_GetMethod("0014084630" , &dst);
//              sprintf( buff , "ID : %s TIMESTAMP : %s VALUE : %s" , dst.id_staff , dst.name_first , dst.name_last );
//              Serial.println( buff );
//        }

//        if ( flag ) 
//        {
//            Serial.println( count ); 
//            flag = 0; 
//        }
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
             String msg = " ";
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

int queryByID_GetMethod( const char * id , employ_TYPE * result) 
{
            String msg = " ";
            char buff[300];
//            char * id_test = "0014084630";
            sprintf( buff , "http://www.bunnam.com/projects/majorette_pp/update/rfid.php?id_rfid=%s" , id );
//            Serial.println( buff );
            msg = httpGETRequest(buff);

             if( msg != "null" ) 
             {

                          Serial.println( msg );
                          Serial.println( msg.length() );
                
                          DynamicJsonDocument  doc( msg.length() + 128 ) ;
                          DeserializationError error = deserializeJson(doc, msg);
                          if (error) 
                          {
                            Serial.print(F("deserializeJson() failed: "));
                            Serial.println(error.f_str());
                            Serial.println(" : ID is not found !");
                            result->flag_err = 0;
                            return -1;
                          }
                          else
                          {
                              const char* id_staff = doc["id_staff"]; // "000002"
                              const char* name_first = doc["name_first"]; // "Thanasin"
                              const char* name_last = doc["name_last"]; // "Bunnam"
              
              //                strcpy( result->id_staff , doc["id_staff"] );
              //                strcpy( result->name_first , doc["name_first"] );
              //                strcpy( result->name_last , doc["name_last"] );
              
                              result->id_staff = strdup(doc["id_staff"]); // "000002"
              //                memcpy( result->id_staff , doc["id_staff"], strlen( doc["id_staff"] ) );
                              result->name_first = strdup(doc["name_first"]); // "Thanasin"
                              result->name_last = strdup(doc["name_last"]); // "Bunnam"
                              result->flag_err = 0 ;
              //                sprintf( buff , "ID : %s TIMESTAMP : %s VALUE : %s" , result->id_staff , result->name_first , result->name_last );
                                sprintf( buff , "%s" , result->id_staff);
                              Serial.println( buff ) ;   
                              Serial.println( "--------------" ) ; 
                              return 0;
                          } 
                
              }
              else
              {
                  result->flag_err = 1;
                  Serial.println("Error!");
                  return -2;
              }


}


void draw_pixel(int16_t x, int16_t y)
{
  display.setColor(WHITE);
  display.setPixel(x, y);
}

void clear_pixel(int16_t x, int16_t y)
{
  display.setColor(BLACK);
  display.setPixel(x, y);
}
