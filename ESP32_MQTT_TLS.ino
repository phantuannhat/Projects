#include <WiFi.h>
#include <WiFiClientSecure.h>  //included WiFiClientSecure does not work!
#include <time.h>
#include <PubSubClient.h>

  const char ssid[] = "COFFEE A SINH";
  const char pass[] = "68686868";

  #define HOSTNAME "HomeBroker"

  const char *MQTT_HOST = "192.168.0.116";
  const uint16_t MQTT_PORT = 8883;
  const char *MQTT_USER = "openhabian"; // leave blank if no credentials used
  const char *MQTT_PASS = "homeautomation@"; // leave blank if no credentials used

  const char* local_root_ca = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIID/zCCAuegAwIBAgIUU2g1bkn6MP5XIT92FJ3+pTD/nVwwDQYJKoZIhvcNAQEL\n" \
    "BQAwgY4xCzAJBgNVBAYTAlZOMRAwDgYDVQQIDAdEYSBOYW5nMRAwDgYDVQQHDAdE\n" \
    "YSBOYW5nMQswCQYDVQQKDAJOTDENMAsGA1UECwwEVEVTVDETMBEGA1UEAwwKSG9t\n" \
    "ZUJyb2tlcjEqMCgGCSqGSIb3DQEJARYbbmhhdGVsZWN0cmljYWxsZWRAZ21haWwu\n" \
    "Y29tMB4XDTIxMDYxMTEzMzExMFoXDTI2MDYxMTEzMzExMFowgY4xCzAJBgNVBAYT\n" \
    "AlZOMRAwDgYDVQQIDAdEYSBOYW5nMRAwDgYDVQQHDAdEYSBOYW5nMQswCQYDVQQK\n" \
    "DAJOTDENMAsGA1UECwwEVEVTVDETMBEGA1UEAwwKSG9tZUJyb2tlcjEqMCgGCSqG\n" \
    "SIb3DQEJARYbbmhhdGVsZWN0cmljYWxsZWRAZ21haWwuY29tMIIBIjANBgkqhkiG\n" \
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEArokSkU5ixGvFG8HVsjLvevkQ3ZjzBPj03zxC\n" \
    "+1khIGpZHDwTJI3r3kiceOQvgzxO+/EYUY8hZf3EXr/+w+LTF09/WvunYNxqz7ff\n" \
    "Qk0Hth3djIx03sDJ3NqnF2+jN+Owc21RYTCvYsIgY4Mr1vWLlMSepN06KedHO/x3\n" \
    "7keZJKOE4CiJfTHOCDYrA45p2c3+/xZ47tRxBQl0bkP9iBwKZc7X9wlhh5r9IR5u\n" \
    "3KKXbYQHXnxsu37oC40JiWWW8JMdZtbjouz9ZfGq6/8WguaoF60gx6n9rsjMrqGg\n" \
    "s47IFSDupRAPcBZdNGAMHPRfDLEJXLPOK3JHlCru9Nqt+0bU5QIDAQABo1MwUTAd\n" \
    "BgNVHQ4EFgQUof5/niQAtCqnUHWUpckAkSeDYh4wHwYDVR0jBBgwFoAUof5/niQA\n" \
    "tCqnUHWUpckAkSeDYh4wDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOC\n" \
    "AQEAOqO6KVDINPxg4wOIKe4Vo0itTMmdy8vIXT4sG0NGd/UXnHXyyfwzSZYzDwml\n" \
    "y/baIc7yd2rMhrpFMPiiePoCTHGVLTafHYFgoBYsPjDms72qapMSG/d5/gtSMrwb\n" \
    "mJfw4mah8QyHXdhNY+GwPmnUixIP9dnadks6f8e9nQdrhwlhDW4Ho4sYXXxhWigs\n" \
    "GWbP/b7XFpCpnI2Mh92G7D7m5mPxi1lYr8/U1vnGT1ivbtteL1Hx0ez1hpEy5w2c\n" \
    "3YYutDytx47mqx9zRToNfp0IHIRhZgVPa2RyKRa0Yc8O8cumUmvSP2nYuhZUXIck\n" \
    "f1WC1qf9mpTkPkqgG948rwRbag==\n" \
    "-----END CERTIFICATE-----";


const char MQTT_SUB_TOPIC[] = "home/" HOSTNAME "/in";
const char MQTT_PUB_TOPIC[] = "home/" HOSTNAME "/out";

WiFiClientSecure net;
PubSubClient client(net);

time_t now;
unsigned long lastMillis = 0;

void mqtt_connect()
{
    while (!client.connected()) {
    Serial.print("Time:");
    Serial.print(ctime(&now));
    Serial.print("MQTT connecting");
    if (client.connect(HOSTNAME, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      client.subscribe(MQTT_SUB_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
  
}

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

void setup()
{
  Serial.begin(115200);

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.setHostname(HOSTNAME);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("Setting time using SNTP");
  configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < 1510592825) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  net.setCACert(local_root_ca);
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(receivedCallback);
  mqtt_connect();
}

void loop()
{
  now = time(nullptr);
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Checking wifi");
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(10);
    }
    Serial.println("connected");
  }
  else
  {
    if (!client.connected())
    {
      mqtt_connect();
    }
    else
    {
      client.loop();
    }
  }

  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    client.publish(MQTT_PUB_TOPIC, ctime(&now), false);
  }
}
