/*-----------------------------------------------------------------*/
/* Project for ESP8266 nodeMCU
   Author: Phan Tuan Nhat
   Title: Get the string of characters from the serial display and control the LED as follows:
          + LED_ON --> Turn on LED
          + LED_OFF --> Turn off LED
          + characters not match --> print to the display "Invalid string"
            */
/*-----------------------------------------------------------------*/
#include <String.h>

#define LED 16

void setup(){
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH)  // turn off LED.
  Serial.begin(115200);
}

String data = "";
void loop(){
  if(Serial.available() > 0){
    data = Serial.readString();
    if(data == "LED_ON")
      digitalWrite(LED, LOW);
     else if(data == "LED_OFF")
      digitalWrite(LED, HIGH);
     else
      Serial.println("Invalid string!");
  }
}
