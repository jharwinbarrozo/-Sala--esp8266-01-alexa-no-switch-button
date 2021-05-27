#include <ESP8266WiFi.h>
#include <Espalexa.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
// define the GPIO connected with Relays and switches
#define RelayPin1 0  //GPIO0
#define wifiLed 2  //GPIO2 This is the built in lED of ESP8266-01/ESP-01
int toggleState_1 = 0; //Define integer to remember the toggle state for relay 1

// prototypes
boolean connectWifi();
//callback functions
void firstLightChanged(uint8_t brightness);

// WiFi Credentials
const char* ssid = "CayganFiber20MBPS";
const char* password = "caygan22";

// device names
String Device_1_Name = "Sala";

boolean wifiConnected = false;

Espalexa espalexa;

//our callback functions
void firstLightChanged(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
    {
      digitalWrite(RelayPin1, LOW);
      Serial.println("Device1 ON");
      toggleState_1 = 1;
    }
  else
  {
    digitalWrite(RelayPin1, HIGH);
    Serial.println("Device1 OFF");
    toggleState_1 = 0;
  }
}

void relayOnOff(int relay){

    switch(relay){
      case 1: 
             if(toggleState_1 == 0){
              digitalWrite(RelayPin1, LOW); // turn on relay 1
              toggleState_1 = 1;
              Serial.println("Device1 ON");
              }
             else{
              digitalWrite(RelayPin1, HIGH); // turn off relay 1
              toggleState_1 = 0;
              Serial.println("Device1 OFF");
              }
             delay(100);
      break;
      default : break;      
      }
}
// connect to wifi – returns true if successful or false if not
boolean connectWifi()
{
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.hostname("Alexa-sala");
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}

void addDevices(){
  // Define your devices here.
  espalexa.addDevice(Device_1_Name, firstLightChanged); //simplest definition, default state off
  espalexa.begin();
}

void setup()
{
  Serial.begin(115200);

  pinMode(RelayPin1, OUTPUT);
  pinMode(wifiLed, OUTPUT);

  //During Starting all Relays should TURN on
  digitalWrite(RelayPin1, LOW);
  
    // Initialise wifi connection
  wifiConnected = connectWifi();
  if (wifiConnected){
    addDevices();
  }
  else {
    Serial.println("Cannot connect to WiFi. So in Manual Mode");
    delay(1000);
  }
   //ArduinoOTA code starts
  ArduinoOTA.onStart([]() {   
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();

}

void loop()
{
  ArduinoOTA.handle();
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(wifiLed, HIGH); //Turn off WiFi LED
  }
  else {
   digitalWrite(wifiLed, LOW); //Turn on WiFi LED
  }
    //WiFi Control
    if (wifiConnected) {
      espalexa.loop();
      delay(1);
    }
    else {
      wifiConnected = connectWifi(); // Initialise wifi connection
      if(wifiConnected){
      addDevices();
    }
  }
}