#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "string.h"

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

typedef enum {

  MC_STANDBY ,
  MC_WORKING ,
  MC_BREAK 
  
  }state_type ;

typedef enum {

  BR_NOBREAK,
  BR_CUTOFF ,
  BR_TOILET ,
  BR_LUNCH
  
  }break_type ;

state_type state_machine = MC_STANDBY ;
break_type state_break ;
employ_touch_TYPE dst;

volatile bool interruptWork;
volatile bool interruptBreak;
int workCounter;
int breakCounter;

volatile bool isCrad = 0;
volatile bool isWork = 0;
volatile bool isBreak = 0;

const int interruptPin = 21;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimerWork() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptWork = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR onTimerBreak() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptBreak = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void iscard() {
  noInterrupts();
  isCrad = 1;
  interrupts();
}

String translate_state_mc( state_type state ) 
{
  switch( state ) 
  {
    case MC_STANDBY : return " STANDBY ! " ;
    case MC_WORKING : return " WORKING ! " ;
    case MC_BREAK : return " BREAK ! " ;  
  } 
}

String translate_hh_mm_cc( int sec ) 
{
    int h, m, s;
    String buff = "" ;

    h = (sec/3600); 
  
    m = (sec -(3600*h))/60;
  
    s = (sec -(3600*h)-(m*60));

    buff += ( h > 9 ) ? "" : "0"; 
    buff += h ;
    buff += ":" ;
    buff += ( m > 9 ) ? "" : "0"; 
    buff += m ;
    buff += ":" ;
    buff += ( s > 9 ) ? "" : "0"; 
    buff += s ;

    return buff;
    
}


const char* ssid = "3bb";
const char* password = "0844025188";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  timer = timerBegin(0, 80, true);
  timerDetachInterrupt(timer);

  attachInterrupt(digitalPinToInterrupt(interruptPin), iscard, FALLING );

//  Serial.println("Input 1 to touch first ");
  
}

int incomingByte ;
int command = 0;

void loop() {

  



    if ( isCrad ) { //Loop mock touch card .
      

              noInterrupts();
              isCrad = 0
              interrupts();

              Serial.println( "---------------------------" );
              Serial.print( "| State : " );
              Serial.print( translate_state_mc(state_machine) );
              Serial.println( " |" );
              Serial.println( "---------------------------" );
          
          if( state_machine == MC_WORKING ) 
            {
                Serial.println("----- Select Break -----");
               Serial.println( " choice 1 : cutoff work " );
               Serial.println( " choice 2 : toilet " );
               Serial.println( " choice 3 : lunch time " );
               isBreak = 1 ;
               Serial.flush();
            }

            else if (state_machine == MC_BREAK )
            {
                Serial.println("----- Cancel break sure ? -----");
                Serial.println( " choice 1         : Yes " );
                Serial.println( " choice otherwise : No " );
                isWork = 1 ;
                Serial.flush();
            }
            else
            {
               timerAttachInterrupt(timer, &onTimerWork, true);
               timerAlarmWrite(timer, 1000000, true);
               timerAlarmEnable(timer);
               /* push DB_1 */
                   if( query_Touch_GetMethod( "02-01" , "0001609206" , &dst ) == 0 )
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
               /* push DB_1 */     
               state_machine = MC_WORKING ;
            }        
      }


    if ( isBreak ) 
    {
        if (Serial.available() > 0) 
        {
            int incomingByte = Serial.read();
            Serial.println(incomingByte, DEC);

            isBreak = 0 ;

            switch ( incomingByte ) 
            {
              case 49 : 
                  state_machine = MC_STANDBY ; 
                  Serial.println( "cutoff work" ); 
                  timerDetachInterrupt(timer);
                  /* DB4 */
                  query_Quit_GetMethod("0001609206" ,"5639407","500","02-01","4","300","0","0");
                  /* DB4 */
                  workCounter = 0;
                  break ;
              
              case 50 : 
                  state_machine = MC_BREAK ; 
                   timerAttachInterrupt(timer, &onTimerBreak, true);
                   timerAlarmWrite(timer, 1000000, true);
                   timerAlarmEnable(timer);
                   state_machine = MC_BREAK ;
                   /* DB3 */
                   query_Break_GetMethod( "0001609206" ,"5639407","500","02-01","1" );
                   /* DB3 */
                  Serial.println( "toilet" ); 
                  break ;
                  
              case 51 : 
                  state_machine = MC_BREAK ; 
                   timerAttachInterrupt(timer, &onTimerBreak, true);
                   timerAlarmWrite(timer, 1000000, true);
                   timerAlarmEnable(timer);
                  state_machine = MC_BREAK ;
                  /* DB3 */
                  query_Break_GetMethod( "0001609206" ,"5639407","500","02-01","2" );
                  /* DB3 */
                  Serial.println( "lunch time" ); 
                  break ;
              
              default : break ;
            }

           
        }

         
    }

    if ( isWork ) 
    {
        if (Serial.available() > 0) 
        {
            int incomingByte = Serial.read();
            Serial.println(incomingByte, DEC);

            isWork = 0 ;

            switch ( incomingByte ) 
            {
              case 49 : 
                    timerAttachInterrupt(timer, &onTimerWork, true);
                    timerAlarmWrite(timer, 1000000, true);
                    timerAlarmEnable(timer);
                    state_machine = MC_WORKING ;
                    /* DB3 */
                    query_Continue_GetMethod("02-01","0001609206");
                    /* DB3 */
                    breakCounter = 0;              
              break ;
              
              case 50 : break ;
              default : break ;
            }

        }
    }
  

    if( interruptWork ) 
    {
      
      portENTER_CRITICAL_ISR(&timerMux);
      interruptWork = 0;
      portEXIT_CRITICAL_ISR(&timerMux);
      
      workCounter++;
      Serial.print("Work : ");
//      Serial.println(workCounter);
      Serial.println(translate_hh_mm_cc(workCounter));
      
 
    }

    if( interruptBreak ) 
    {
      
      portENTER_CRITICAL_ISR(&timerMux);
      interruptBreak = 0;
      portEXIT_CRITICAL_ISR(&timerMux);
      
      breakCounter++;
      Serial.print("Break : ");
//      Serial.println(breakCounter);
      Serial.println(translate_hh_mm_cc(breakCounter));
 
    }


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



int query_Continue_GetMethod( const char * id_mc , const char * id_rfid  ) 
{
    String msg = " ";
    char buff[300];
    sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/continue_v2.php?id_mc=%s&id_rfid=%s" , id_mc , id_rfid);
    Serial.println(buff);
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
              Serial.println("Error Continue!");
              return -1;
          }
          if(doc["code"])
          {
              Serial.print("CODE : ");
              Serial.println((const char *)(doc["code"]));
              return -3;
          }
          if( doc["total_break"] ) 
          {
            Serial.println((const char *)(doc["total_break"]));
            return 0;
          }
  }
  else
  {
    Serial.println("Error!");
    return -2;    
  }
}



int query_Break_GetMethod( char * id_rfid,char * id_job , char * operation , char * id_mc , char * break_code ) 
{
    String msg = " ";
    char buff[300];
    sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/break_v2.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s&break_code=%s" ,id_rfid,id_job,operation,id_mc,break_code );
    Serial.println(buff);
    msg = httpGETRequest(buff);

    if ( msg != "null" )
    {

          Serial.println( msg );
          Serial.println( msg.length() );


      if( msg == "OK" ) 
        {
            return 0;
        }
        else
        {
           DynamicJsonDocument  doc( msg.length() + 256 ) ;
           DeserializationError error = deserializeJson(doc, msg);
            if (error)
            {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              Serial.println("Error Break!");
              return -1;
            }
            if(doc["code"])
            {
              Serial.print("CODE : ");
              Serial.println((const char *)(doc["code"]));
              return -3;
            }
         }
      

  }
  else
  {
    Serial.println("Error!");
    return -2;    
  }
}


int query_Quit_GetMethod( char* id_rfid,char * id_job , char * operation , char * id_machine , char  * no_send , char * no_pulse1 ,char * no_pulse2 , char * no_pulse3 ) 
{
    String msg = " ";
    char buff[300];
    sprintf( buff , "http://bunnam.com/projects/majorette_pp/update/quit_v2.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s&no_send=%s&no_pulse1=%s&no_pulse2=%s&no_pulse3=%s" ,id_rfid,id_job,operation,id_machine,no_send,no_pulse1,no_pulse2,no_pulse3 );
    Serial.println(buff);
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
              Serial.println("Error Quit!");
              return -1;
          }
          if(doc["code"])
          {
              Serial.print("CODE : ");
              Serial.println((const char *)(doc["code"]));
              return -3;
          }
          if( doc["time_work"] ) 
          {
            Serial.println((const char *)(doc["time_work"]));
            return 0;
          }
  }
  else
  {
    Serial.println("Error!");
    return -2;    
  }
}
