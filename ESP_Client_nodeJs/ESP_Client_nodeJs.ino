/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

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
} employ_TYPE ;

typedef struct date_t {
  char* date_;
  char* time_;
} datetime_TYPE ;

SSD1306   display(0x3c, 21, 22); //SDA - SCL
extern dw_font_info_t font_th_sarabun_new_regular14;
dw_font_t myfont;

volatile uint32_t count;
int8_t flag = 0;
char buff[300];
String msg;


static employ_TYPE  dst;
datetime_TYPE date_time;

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

  for (uint8_t t = 3; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  queryDate_Time(&date_time);
  Serial.println( date_time.date_ );
  Serial.println( date_time.time_ );

}


void loop() {
  if (WiFi.status() == WL_CONNECTED )
  {
    /*  //--------------------------------------------
      while ( Serial.available() ) {
         msg = Serial.readString();

        if(  queryByID_GetMethod( ( const char * )msg.c_str() , &dst) == 0 ){

          //             queryByID_GetMethod("0014084630" , &dst);
         sprintf( buff , "ID : %s TIMESTAMP : %s VALUE : %s" , dst.id_staff , dst.name_first , dst.name_last );
         Serial.println( buff );


          display.resetDisplay();

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

          }
          else
          {

          }


      }
      //--------------------------------------------
    */



    //        if ( flag )
    //        {
    //            Serial.println( count );
    //            flag = 0;
    //        }


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

  if (httpResponseCode > 0) {
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

  if (httpResponseCode > 0) {
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


int queryByID_GetMethod( const char * id , employ_TYPE * result)
{
  String msg = " ";
  char buff[300];
  sprintf( buff , "http://www.bunnam.com/projects/majorette_pp/update/rfid.php?id_rfid=%s" , id );
  msg = httpGETRequest(buff);

  if ( msg != "null" )
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
//      const char* id_staff = doc["id_staff"]; // "000002"
//      const char* name_first = doc["name_first"]; // "Thanasin"
//      const char* name_last = doc["name_last"]; // "Bunnam"
      result->id_staff = strdup(doc["id_staff"]); // "000002"
      result->name_first = strdup(doc["name_first"]); // "Thanasin"
      result->name_last = strdup(doc["name_last"]); // "Bunnam"
      result->flag_err = 0 ;
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

int queryDate_Time( datetime_TYPE * date )
{
  char buff[100];
  String msg = " ";
  
  sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/time.php" );

  msg = httpGETRequest(buff);
  Serial.println(msg);

  DynamicJsonDocument  doc( msg.length() + 128 ) ;
  DeserializationError error = deserializeJson(doc, msg);

  if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      Serial.println("URL OR Server Fail!");
      return -1;
    }
    else
    {
//      const char* date_temp = doc["date"]; // "23-07-2021"
//      const char* time_temp = doc["time"]; // "20:47:19"

      date->date_ = strdup(doc["date"]);
      date->time_ = strdup(doc["time"]);
//      Serial.println( date->date_ ) ;
//      Serial.println( date->time_ ) ;    
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
