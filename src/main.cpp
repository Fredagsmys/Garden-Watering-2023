#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define waterPin D0 
#define buttonPin D4 
#define RedLedPin D6 
#define GreenLedPin D7
#define moistPin A0

WiFiClient wifiClient;
const char* SSID     = "Karsjo_1_54";
const char* PASSWORD = "92ac437921";
int waterStatus = false;
int buttonState;
const int GET_INTERVAL = 5000;
const int WATER_DURATION = 120000;
const int SEND_INTERVAL = 60000;
unsigned long previous_post_millis = millis();
unsigned long previous_get_millis = millis();

void CheckWaterStatus() {
  if (WiFi.status() == WL_CONNECTED) {  //Check WiFi connection status


    HTTPClient http;                                       //Declare object of class HTTPClient
    http.begin(wifiClient,"http://mattsson.servehttp.com:5000/get_water_status");
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      String payload = http.getString();
      if (payload == "1"){
        waterStatus = true;
      }
    }
    http.end();                      //Close connection
  } else {
    //conect to wifi
  }
}

void PostHttpToServer(int value, int sensor_type, int sensor_id) {//ändra till skicka en request med alla tre sensordata
  if (WiFi.status() == WL_CONNECTED) {  //Check WiFi connection status


    HTTPClient http;                                       //Declare object of class HTTPClient
    http.begin(wifiClient,"http://mattsson.servehttp.com:5000/send_data");
    http.addHeader("Content-Type", "application/json");
    char json_data[150];
    sprintf(json_data, "{\"value\":%d, \"sensorType\":%d, \"sensorID\": %d}", value, sensor_type,sensor_id);
    int httpResponseCode = http.POST(json_data);
    if (httpResponseCode == 200) {
      digitalWrite(GreenLedPin,HIGH);
      delay(1000);
      digitalWrite(GreenLedPin,LOW);
    }
    http.end();                      //Close connection
  } else {
    //connect to wifi
  }
}



void setup() {
  // Serial.begin(9600);
  // while(!Serial);
  // Serial.println("SERIAL STARTED");
  pinMode(waterPin,OUTPUT);
  pinMode(buttonPin,INPUT);
  pinMode(RedLedPin,OUTPUT);
  pinMode(GreenLedPin,OUTPUT);
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }
  
}

void WaterGarden(){

    digitalWrite(waterPin,HIGH);
    digitalWrite(GreenLedPin,HIGH);
    digitalWrite(RedLedPin,HIGH);
    
    delay(WATER_DURATION);

    digitalWrite(waterPin,LOW);
    digitalWrite(GreenLedPin,LOW);
    digitalWrite(RedLedPin,LOW);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  // Serial.println("buttonState: " + buttonState);
  // Serial.println("waterStatus: " + waterStatus);
  delay(500);
  if (buttonState == LOW || waterStatus){
    waterStatus = false;
    WaterGarden();
  }

  if (millis() - previous_get_millis > GET_INTERVAL)
  {
    CheckWaterStatus();
    previous_get_millis = millis();
  }

  if (millis() - previous_post_millis > SEND_INTERVAL)
  {
    
    PostHttpToServer(analogRead(moistPin), 3, 2);
    previous_post_millis = millis();
  }
  
}