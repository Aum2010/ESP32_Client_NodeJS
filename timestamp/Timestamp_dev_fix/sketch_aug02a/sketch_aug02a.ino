
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
                Serial.println("----- Break Sure ? -----");
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
      Serial.println(workCounter);
 
    }

    if( interruptBreak ) 
    {
      
      portENTER_CRITICAL_ISR(&timerMux);
      interruptBreak = 0;
      portEXIT_CRITICAL_ISR(&timerMux);
      
      breakCounter++;
      Serial.print("Break : ");
      Serial.println(breakCounter);
 
    }


}
