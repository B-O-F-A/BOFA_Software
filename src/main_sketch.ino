
#include "globals.h"


// the setup function runs once when you press reset or power the board
void setup() {
  
  controller_Mailbox=xQueueCreate(1, sizeof(int));
  AnalogRead_Mailbox=xQueueCreate(1, sizeof(int));
  Serial.begin(9600);


  xTaskCreate(
    controller
    , "Blink" // A name just for humans
    , 128 // This stack size can be checked & adjusted by reading the Stack Highwater
    , NULL
    , 1 // Priority, with 1 being the highest, and 4 being the lowest.
    , NULL );

  xTaskCreate(
    TaskAnalogRead
    ,"AnalogRead"
    , 128 // Stack size
    , NULL
    , 1 // Priority
    , NULL );

  Serial.println("setup complete: starting.....");
  vTaskDelay(pdMS_TO_TICKS(100));
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}



void loop()
{
  taskYIELD();
}

//
//void TaskAnalogRead(void *pvParameters)
//{
//  (void) pvParameters;
//  // initialize serial communication at 9600 bits per second:
//  int recieved_val = 0;
//  for (;;)
//  {
//    if(xQueueReceive(AnalogRead_Mailbox,&recieved_val,portMAX_DELAY) == pdPASS ){
//      Serial.print(millis());
//      Serial.print("          ");
//      Serial.println(recieved_val);
//    }
//  }
//}
//
//void controller(void *pvParameters)
//{
//  (void) pvParameters;
//
//  //char buff[100];
//  for (;;)
//  {
//    int time1 = millis();
//    xQueueSend(AnalogRead_Mailbox,&time1,portMAX_DELAY);
//    vTaskDelay(30);
//  }
//}
