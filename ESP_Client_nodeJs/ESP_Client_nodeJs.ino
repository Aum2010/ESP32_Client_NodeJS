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

#define PIN_COUNTER 5

unsigned long period_get = 1000;
unsigned long period_post = 3000;
unsigned long last_time_get = 0;
unsigned long last_time_post = 0;


const char* ssid = "3bb";
const char* password = "0844025188";

typedef struct employ_touch {
  char* id_staff ;
  char* name_first ;
  char* name_last ;
  int role ;
  char * id_task ;
  char * id_job ;
  char * item_no ;
  char* operation ;
  char * op_name ;
  char * qty_order ;
  char * qty_comp ;
  char * qty_open ;
  uint8_t flag_err ;
} employ_touch_TYPE ;

typedef struct date_t {
  char* date_;
  char* time_;
} datetime_TYPE ;

typedef enum 
{
  PROCESS_OK ,
  PROCESS_ERR_CODE_GET ,
  PROCESS_ERR_
} err_type;


SSD1306   display(0x3c, 21, 22); //SDA - SCL
extern dw_font_info_t font_th_sarabun_new_regular14;
dw_font_t myfont;

volatile uint32_t count;
int8_t flag = 0;
char buff[300];
String msg;


static employ_touch_TYPE dst;
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

//  queryDate_Time(&date_time);
//  Serial.println( date_time.date_ );
//  Serial.println( date_time.time_ );

  //query_Touch_GetMethod( "02-02" , "0001609206" , &dst );

/* *Fail Case* */
  if( query_Touch_GetMethod( "00-02" , "0001609206" , &dst ) == 0 )
  {
  Serial.println( dst.id_staff );
  Serial.println( dst.name_first );
  Serial.println( dst.name_last );
  Serial.println( dst.role );
  Serial.println( dst.id_task );
  Serial.println( dst.id_job );
  Serial.println( dst.operation );
  Serial.println( dst.op_name );
  Serial.println( dst.qty_order );
  Serial.println( dst.qty_comp );
  Serial.println( dst.qty_open );
  }

/* *Good Case* */
    if( query_Touch_GetMethod( "02-02" , "0001609206" , &dst ) == 0 )
  {
  Serial.println( dst.id_staff );
  Serial.println( dst.name_first );
  Serial.println( dst.name_last );
  Serial.println( dst.role );
  Serial.println( dst.id_task );
  Serial.println( dst.id_job );
  Serial.println( dst.operation );
  Serial.println( dst.op_name );
  Serial.println( dst.qty_order );
  Serial.println( dst.qty_comp );
  Serial.println( dst.qty_open );
  }

  
  //query_Count_GetMethod( "5639407","545","02-02",(int *)8,(int *)1,(int *)1,(int *)1 );
  
}


void loop() {
  if (WiFi.status() == WL_CONNECTED )
  {
    
/*      //--------------------------------------------
      while ( Serial.available() ) {
         msg = Serial.readString();

        if(  queryByID_GetMethod( ( const char * )msg.c_str() , " " , &dst) == 0 ){

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
      //-------------------------------------------- */
    


//
//            if ( flag )
//            {
//                Serial.println( count );
//                flag = 0;
//            }


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


int query_Touch_GetMethod( const char * id_mc , const char * id_rfid , employ_touch_TYPE * result)
{
  String msg = " ";
  char buff[300];
  sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/touch.php?id_mc=%s&id_rfid=%s" , id_mc , id_rfid );
  msg = httpGETRequest(buff);

  if ( msg != "null" )
  {
    Serial.println( msg );
    Serial.println( msg.length() );

    DynamicJsonDocument  doc( msg.length() + 256 ) ;
    DeserializationError error = deserializeJson(doc, msg);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      Serial.println(" : ID is not found !");
      result->flag_err = 0;
      return -1;
    }
    else if(doc["code"])
    {
      Serial.print("CODE : ");
      Serial.println((const char *)(doc["code"]));
      return -3;
    }
    else
    {

      result->id_staff = strdup(doc["id_staff"]); // "000002"
      result->name_first = strdup(doc["name_first"]); // "Thanasin"
      result->name_last = strdup(doc["name_last"]); // "Bunnam"
      result->role = doc["role"]; 
      result->id_task = strdup(doc["id_task"]); // "8"
      result->id_job = strdup(doc["id_job"]); 
      result->item_no = strdup(doc["item_no"]);
      result->operation = strdup(doc["operation"]);
      result->op_name = strdup(doc["op_name"]); 
      result->qty_order = strdup(doc["qty_order"]);
      result->qty_comp = strdup(doc["qty_comp"]);
      result->qty_open = strdup(doc["qty_open"]); 
      result->flag_err = 0 ;
      //sprintf( buff , "%s" , result->id_staff);
      //Serial.println( buff ) ;
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

int query_Count_GetMethod( char * id_job , char * operation , char * id_machine , int * no_send , int* no_pulse1 ,int* no_pulse2 , int* no_pulse3 ) 
{
    String msg = " ";
    char buff[300];
    sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/count.php?id_job=%s&operation=%s&id_machine=%s&no_send=%d&no_pulse=%d&no_pulse2=%d&no_pulse3=%d" ,id_job,operation,id_machine,no_send,no_pulse1,no_pulse2,no_pulse3 );
    Serial.println(buff);
//    msg = httpGETRequest(buff);
}

int query_Break_GetMethod( char * id_breakcard,char * id_job , char * operation , char * id_machine ) 
{
    String msg = " ";
    char buff[300];
    sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/break.php?id_breakcard=%s&id_job=%s&operation%s&id_machine=%s" ,id_breakcard,id_job,operation,id_machine );
    Serial.println(buff);
//    msg = httpGETRequest(buff);
}

int query_Quit_GetMethod( char* id_rfid,char * id_job , char * operation , char * id_machine , int * no_send , int* no_pulse1 ,int* no_pulse2 , int* no_pulse3 , int * time_work ) 
{
    String msg = " ";
    char buff[300];
    sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/quit.php?id_rfid=%s&id_job=%s&operation=%s&id_machine=%s&no_send=%d&no_pulse1=%d&no_pulse2=%d&no_pulse3=%d" ,id_rfid,id_job,operation,id_machine,no_send,no_pulse1,no_pulse2,no_pulse3 );
    Serial.println(buff);
//    msg = httpGETRequest(buff);
}

int query_Quit_Rs_GetMethod( char* id_rfid,char * id_job , char * operation , char * id_machine , int * no_send , int* no_pulse1 ,int* no_pulse2 , int* no_pulse3 ,char * id_rs, int * qty_rs ,int * time_work ) 
{
    String msg = " ";
    char buff[300];
    sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/quit-rs.php?id_rfid=%s&id_job=%s&operation=%s&id_machine=%s&no_send=%d&no_pulse1=%d&no_pulse2=%d&no_pulse3=%d&id_rs=%d&qty_rs=%d" ,id_rfid,id_job,operation,id_machine,no_send,no_pulse1,no_pulse2,no_pulse3,id_rs,qty_rs );
    Serial.println(buff);
//    msg = httpGETRequest(buff);
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


int queryByID_GetMethod( const char * id_mc , const char * id_rfid , employ_touch_TYPE * result)
{
  String msg = " ";
  char buff[300];
  sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/touch.php?id_mc=%s&id_rfid=%s" , id_mc , id_rfid );
  msg = httpGETRequest(buff);

  if ( msg != "null" )
  {
    Serial.println( msg );
    Serial.println( msg.length() );

    DynamicJsonDocument  doc( msg.length() + 256 ) ;
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

      result->id_staff = strdup(doc["id_staff"]); // "000002"
      result->name_first = strdup(doc["name_first"]); // "Thanasin"
      result->name_last = strdup(doc["name_last"]); // "Bunnam"
      result->role = doc["role"]; 
      result->id_task = strdup(doc["id_task"]); // "8"
      result->id_job = strdup(doc["id_job"]); 
      result->item_no = strdup(doc["item_no"]);
      result->operation = strdup(doc["operation"]);
      result->op_name = strdup(doc["op_name"]); 
      result->qty_order = strdup(doc["qty_order"]);
      result->qty_comp = strdup(doc["qty_comp"]);
      result->qty_open = strdup(doc["qty_open"]); 
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
