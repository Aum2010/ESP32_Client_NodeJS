volatile bool interruptWork;
volatile bool interruptBreak;
int workCounter;
int breakCounter;

 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

struct break_count {
    int break_start;
    int break_stop;
    struct break_count * bc_next;
  } ;

struct break_count* head = NULL;

 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptWork = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR onTimer2() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptBreak = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}
 
void setup() {
 
  Serial.begin(115200);
 
  timer = timerBegin(0, 80, true);
//  timerAttachInterrupt(timer, &onTimer, true);
  timerDetachInterrupt(timer);
//                 timerAlarmWrite(timer, 1000000, true);
//               timerAlarmEnable(timer);


//  push(&head, 0 , 60);
//  push(&head, 0 , 50);
//  push(&head, 0 , 80);
//
//  printList(head);
 
}

volatile int memo ;
 
void loop() {


     if (Serial.available() > 0) {
          int inByte = Serial.read();
          
          Serial.println(inByte);

          if( inByte == memo ) 
          {
            return ;
          }
        
          if( inByte == 49 )
          {
               if( memo == 50 ) //if stop break condition
               {
                  push(&head, 0 , breakCounter );
               } 
               
               memo = 49;
               timerAttachInterrupt(timer, &onTimer, true);
               timerAlarmWrite(timer, 1000000, true);
               timerAlarmEnable(timer);
          }
          else if ( inByte == 50 ) 
          {
               if( memo != 49 ) //if must come from stage 49 only!
               {
                  return ;
               } 
               
               memo = 50;
               breakCounter = 0;
               timerAttachInterrupt(timer, &onTimer2, true);
               timerAlarmWrite(timer, 1000000, true);
               timerAlarmEnable(timer);
          }
          else if ( inByte == 48 ) 
          {
               memo = 48;
               timerDetachInterrupt(timer);
               workCounter = 0;
               printList(head);
               deleteList(&head);
          }
      }

/*
    if( workCounter > 10 )
    {
        workCounter = 0;
        timerAttachInterrupt(timer, &onTimer2, true);
    }

    if( breakCounter > 10)
    {
        breakCounter = 0;
        timerAttachInterrupt(timer, &onTimer, true);
    }
 */
 
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

void push(struct break_count** head_ref, int start_b , int stop_b)
{
    /* allocate node */
    struct break_count* new_node =(struct break_count*) malloc(sizeof(break_count));
    /* put in the key  */
    new_node->break_start  = start_b;
    new_node->break_stop  = stop_b;
    /* link the old list off the new node */
    new_node->bc_next = (*head_ref);
    /* move the head to point to the new node */
    (*head_ref)    = new_node;
}

void printList(struct break_count *node)
{
  struct break_count *temp;
  
  while (node != NULL)
  {
     temp = node;

     Serial.print( "Debug TEMP : " );
     Serial.println( temp->break_stop );
     
     Serial.print(node->break_start);
     Serial.print(" -> ");
     Serial.println(node->break_stop);
     Serial.println("---------------");
     node = node->bc_next;
     
     if( temp->bc_next != NULL ) 
     {
        node = NULL ;
        free(temp);
     }
    
  }
}

void deleteList(struct break_count **head_ref)
{
   /* deref head_ref to get the real head */
   struct break_count* current = *head_ref;
   struct break_count* next;
 
   while (current != NULL)
   {
       next = current->bc_next;
       free(current);
       current = next;
   }
   
   /* deref head_ref to affect the real head back
      in the caller. */
   *head_ref = NULL;
}
