
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
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
                  workCounter = 0;
                  break ;
              
              case 50 : 
                  state_machine = MC_BREAK ; 
                   timerAttachInterrupt(timer, &onTimerBreak, true);
                   timerAlarmWrite(timer, 1000000, true);
                   timerAlarmEnable(timer);
                   state_machine = MC_BREAK ;
                  Serial.println( "toilet" ); 
                  break ;
                  
              case 51 : 
                  state_machine = MC_BREAK ; 
                   timerAttachInterrupt(timer, &onTimerBreak, true);
                   timerAlarmWrite(timer, 1000000, true);
                   timerAlarmEnable(timer);
                  state_machine = MC_BREAK ;
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
