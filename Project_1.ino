/*-----------------------------------------------------------------*/
/* Project for ESP8266 nodeMCU
   Author: Phan Tuan Nhat
   Title: When power on: LED 16 blinks with cycle of half a second (0.5s) and LED 2 on.
          affer that if you press button for 2 sesconds --> change Cycle of LED 16 (1S --> 0.5s or 0.5 --> 1s)
          or button press time is less than 2 seconds --> LED 2 changes status.
            */
/*-----------------------------------------------------------------*/

#define LED 16
#define BUTTON 0

/*---------------------Function Prototype--------------------------*/
ICACHE_RAM_ATTR void changeCycle();


void setup() {
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON), changeCycle, FALLING);
}

int getTime, half_Cycle = 250;
bool current_Cycle, last_Cycle;
bool interrupt_Occurred = false;

void loop() {
  digitalWrite(LED, !digitalRead(LED));
  delay(half_Cycle);
  
  if(interrupt_Occurred == true){
    if(current_Cycle != last_Cycle){
      if(half_Cycle == 250)
        half_Cycle = 500;
      else
        half_Cycle = 250;
      current_Cycle = last_Cycle;
    }else
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        
    Serial.print("LED 2 status = ");
    Serial.println(digitalRead(LED_BUILTIN));
    Serial.print("Cycle of LED 16 = ");
    Serial.println(half_Cycle * 2);
    interrupt_Occurred = false;
  }
}

/*----------------------Functions---------------------*/

ICACHE_RAM_ATTR void changeCycle(){
  interrupt_Occurred = true;
  getTime = millis();
  while(digitalRead(BUTTON) == LOW){
    if(millis() >= getTime + 2000){
      current_Cycle = !current_Cycle;
      break;
    }
  }
}
