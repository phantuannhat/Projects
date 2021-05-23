/*-----------------------------------------------------------------*/
/* Tittle:
 * Author: Phan Tuan Nhat

*/
/*-----------------------------------------------------------------*/

/*---------------------------Includes------------------------------*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "DHT.h"
#include <Adafruit_NeoPixel.h>

/*----------------------Declare variables--------------------------*/

#define RGB        D5
#define NUMPIXELS  2

#define DHTPIN     D3
#define DHTTYPE    DHT21

#define Light      D6
#define Fan        D7



//Set up for Wifi and MQTT server
const char* ssid = "PHAN TUNG DUONG";
const char* password = "15022008";
const char* mqtt_server = "192.168.1.9";

//MQTT topics
//char* RGB_topic   = "cmd/gGF/LivingRoom/RGB";
//char* Light_topic = "cmd/gGF/LivingRoom/Light";
//char* Fan_topic   = "cmd/gGF/LivingRoom/Fan";

uint8_t Color_code[3] = {0, 0, 0}; 
bool Random_effects =false;
uint32_t  intervalDHT = 0;
char buff[10];

/*---------------------Function Prototype--------------------------*/
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);


WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(18, RGB, NEO_RGB + NEO_KHZ800);

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  pinMode(Light, OUTPUT);
  pinMode(Fan,   OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  strip.begin(); // This initializes the NeoPixel library.
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();       

  colorWipe(strip.Color(Color_code[0], Color_code[1], Color_code[2]), 5); 

//  rainbow(20);
  
  if((intervalDHT + 10000) < millis()){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
   
    Serial.println();
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));

    sprintf(buff, "%.2f", t);
    client.publish("stat/gGF/LivingRoom/temp", buff);
    sprintf(buff, "%.2f", h);
    client.publish("stat/gGF/LivingRoom/hum", buff);
    intervalDHT = millis();  
  }

    
}

/*--------------------------Functions------------------------------*/

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String get_msg = ""; 
  for (int i = 0; i < length; i++) {
    get_msg += (char)payload[i];
  } 
  Serial.println(get_msg);

  if(strcmp(topic,"cmd/gGF/LivingRoom/Fan") == 0){
    if(get_msg == "ON")
      digitalWrite(Fan, HIGH);
    else
      digitalWrite(Fan, LOW);
  }
  else if(strcmp(topic,"cmd/gGF/LivingRoom/Light") == 0){
    if(get_msg == "ON")
      digitalWrite(Light, HIGH);
    else
      digitalWrite(Light, LOW);
  }
  else if(strcmp(topic,"cmd/gGF/LivingRoom/RGB") == 0){
    if(get_msg == "ON")
      Random_effects = true;
    else{
      Random_effects = false;
      get_msg = "";
      int order = 0;
      for(int i = 0; i <= length; i++){
        if(isDigit((char)payload[i]) && i < length){
          get_msg += (char)payload[i];
        }else{
          Color_code[order] = get_msg.toInt();
          get_msg = "";
          order++;
        }
      } 
      order = 0;
    }
  }
  get_msg = "";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP_MQTT_WS2812B";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Subcrible the topics
      client.subscribe("cmd/gGF/LivingRoom/RGB");
      client.subscribe("cmd/gGF/LivingRoom/Light");
      client.subscribe("cmd/gGF/LivingRoom/Fan");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


//------------------------Neo Pixels--------------------------
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
