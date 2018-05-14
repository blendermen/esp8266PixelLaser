#define DEBUG_SERIAL //uncomment for Serial debugging statements

#ifdef DEBUG_SERIAL
#define DEBUG_BEGIN Serial.begin(115200)
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x) 
#define DEBUG_BEGIN
#endif

//includes
#include "sweeper.h"
#include <PersWiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266SSDP.h>
#include <WebSocketsServer.h>

//extension of ESP8266WebServer with SPIFFS handlers built in
#include <SPIFFSReadServer.h> // https://github.com/r-downing/SPIFFSReadServer
// upload data folder to chip with Arduino ESP8266 filesystem uploader
// https://github.com/esp8266/arduino-esp8266fs-plugin

#include <DNSServer.h>
#include <FS.h>

#define DEVICE_NAME "ESP8266 DEVICE"


//server objects
SPIFFSReadServer server(80);
DNSServer dnsServer;
PersWiFiManager persWM(server, dnsServer);
WebSocketsServer webSocket = WebSocketsServer(81); 
Sweeper sweeper1(10);
Sweeper sweeper2(10);
////// Sample program data
int x;
String y;
int xpos;
int ypos;
 int xEngine;
         int yEngine;
bool isConnectedToWifi = false;
bool engineManualControllMovement = true;;
bool stateON = true;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      //Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
       // Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        //rainbw = false;                  // Turn rainbow off when a new connection is established
        // send message to client
        String initStateMessage = String("S"+String(stateON));
        webSocket.sendTXT(num, initStateMessage);
        
      }
      break;
    case WStype_TEXT:                     // if new text data is received
     // Serial.printf("[%u] get Text from webSocket Json: %s\n", num, payload);
      String payload_str = String((char*) payload);
      StaticJsonBuffer<200> jsonBuffer1;
      JsonObject& root = jsonBuffer1.parseObject(payload_str);
     xpos          = root["x"];
     ypos        = root["y"];
      //Serial.printf("website relative x: %d\n", xpos);
      //Serial.printf("website relative y: %d\n", ypos);

       
     
      if(xpos>0){
         xEngine = 180 - map(xpos, 1, 90, 91, 180);
      }else if(xpos<0)
      {
         xEngine = 180 - map(xpos, -90, -1, 0, 89);
      }else{
        xEngine = 90;
        }

      if(ypos>0){
         yEngine = 180 -  map(ypos, 1, 90, 91, 180);
      }else if(ypos<0)
      {
         yEngine = 180 - map(ypos, -90, -1, 0, 89);
      }else{
        yEngine = 90;
        }
       //Serial.printf("Engine relative x: %d y: %d \n", xEngine,yEngine); 
         // Serial.println();
          // Serial.printf("payload %c \n", (char*)payload[0]); 
           // Serial.printf("payload %s \n", payload[1]); 
      
     // we get other control data i.e. #1 - toggle manual/automatic engine movement
      if ((char)payload[0] == 'M') {            
         if((char)payload[1] == '1'){
          engineManualControllMovement = true;
           Serial.printf("engineManualControllMovement=true");
          }else if((char)payload[1] == '0'){
            engineManualControllMovement = false;
             Serial.printf("engineManualControllMovement=false");
            }
      }

      if ((char)payload[0] == 'S') {            
         if((char)payload[1] == '1'){
          stateON = true;
           Serial.printf("stateON=%d",stateON);
            String initStateMessage = String("S"+String(stateON));
           webSocket.sendTXT(num, initStateMessage);
          }else if((char)payload[1] == '0'){
             stateON = false;
           Serial.printf("stateON=%d",false);
            String initStateMessage = String("S"+String(stateON));
        webSocket.sendTXT(num, initStateMessage);
            }
      }
      
      break;
  }
}

void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
}



//
//SETUP


void setup() {
  DEBUG_BEGIN; //for terminal debugging
  DEBUG_PRINT();
 // WiFi.disconnect();
  //ESP.reset();
  
 sweeper1.Attach(14);
 sweeper2.Attach(13);

   
  startWebSocket();            // Start a WebSocket server
  
   //optional code handlers to run everytime wifi is connected...
  persWM.onConnect([]() {
    DEBUG_PRINT("wifi connected");
    DEBUG_PRINT(WiFi.localIP());
    Serial.printf("bool value is %d",isConnectedToWifi);
   
  });
  //...or AP mode is started
  persWM.onAp([](){
    DEBUG_PRINT("AP MODE");
    DEBUG_PRINT(persWM.getApSsid());
    isConnectedToWifi = true;;
    Serial.printf("bool value is %d",isConnectedToWifi);
  });
  //make connecting/disconnecting non-blocking
  persWM.setConnectNonBlock(true);
  //allows serving of files from SPIFFS
  SPIFFS.begin();
  //sets network name for AP mode
  persWM.setApCredentials(DEVICE_NAME);
  //persWM.setApCredentials(DEVICE_NAME, "password"); optional password
  persWM.begin();

  //handles commands from webpage, sends live data in JSON format
  server.on("/api", []() {
    DEBUG_PRINT("server.on /api");
    if (server.hasArg("x")) {
      x = server.arg("x").toInt();
      DEBUG_PRINT(String("x: ")+x);
    } //if
    if (server.hasArg("y")) {
      y = server.arg("y");
      DEBUG_PRINT("y: "+y);
    } //if

    //build json object of program data
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["x"] = x;
    json["y"] = y;

    char jsonchar[200];
    json.printTo(jsonchar); //print to char array, takes more memory but sends in one piece
    server.send(200, "application/json", jsonchar);

  }); //server.on api

 server.on("/inline", [](){
   Serial.printf("cbool value is %d",isConnectedToWifi);
  if(!isConnectedToWifi){
    server.send(200, "text/plain", "this works as well");
    }
  });

   server.on("/inline2", [&]() {
      server.send(200, "text/plain", "this works as well1");
  });

    server.on("/inline2.htm", [&]() {
      server.send(200, "text/plain", "this works as well2");
  });

  server.begin();
  DEBUG_PRINT("setup complete.");
} //void setup
long int lastInterval = 0;
int interval = 1000;
int count = 0;
int lastPosX = xEngine;
int lastPosY = yEngine;
void loop() {
  count++;
 
  
    webSocket.loop();                           // constantly check for websocket events
 // dnsServer.processNextRequest();
  server.handleClient();
   persWM.handleWiFi();
   
// if(millis() > lastInterval + interval){
//  lastInterval = millis();
//  Serial.printf("Interval test %d \n",millis());
//  }

  if(stateON){
     // moveCount++;
       //int pozycja = random(5,70);
    // zmienPozycje(pion, pozycja); 
    if(!engineManualControllMovement){
     sweeper1.Update();
     sweeper2.Update();
     }else{
          if(count%2==0){
               if(lastPosX != xEngine ){
               sweeper1.UpdateManual(xEngine);
               Serial.printf("zmien pozycje - A %i \n",xEngine);
               lastPosX = xEngine;
             }
          }else{
               if(lastPosY != yEngine ){
               sweeper2.UpdateManual(yEngine);
               Serial.printf("zmien pozycje - B %i \n",yEngine);
               lastPosY = yEngine;
            }
          }
      
      }
     //sweeper2.Update();
     //sweeper2.Update();
     //Serial.printf("zmien pozycje - pion %i \n",moveCount);
      
      //pozycja = random(45,45);
      //zmienPozycje(poziom, pozycja);
     // Serial.printf("zmien pozycje - poziom %i \n",moveCount);
    
     // delay(50);
   // }
  }
  // do stuff with x and y

} //void loop

