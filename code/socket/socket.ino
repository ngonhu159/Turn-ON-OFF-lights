#include <ArduinoJson.h>

#include <ESP8266WiFi.h>

#include <SocketIoClient.h>

#include <EEPROM.h>

SocketIoClient webSocket;

const char* Host_Socket = "tanvuong.herokuapp.com/";

//const char* Host_Socket = "192.168.137.1";
//unsigned int Port_Socket = 3000;
unsigned int Port_Socket=80;
const char* ssid = "Wifi Chua A1";
const char* pwdWifi = "12345678";
                                 
/*==========================================================================================*/
#define ON 1
#define OFF 0

#define buttonLed D3 
#define buttonFan D4 

#define Led D5
#define Fan D6
/*==========================================================================================*/
byte ledStatus = OFF; 
byte fanStatus = OFF; 
int valStatusled=0;  
int valStatusfan=0;   

int beginTime = 0;
int endTime = 0;
/*=============================================Function=============================================*/
void checkStatus();
void checkStatus_led();
void checkStatus_fan();

void checkButton();
void checkButton_led();
void checkButton_fan();

void handlWifidisconnect();       // khi chua ket noi duoc voi wifi thi su dung nut nhan de bat tat    

/*============================================ Event Socket IO ==============================================*/



//void handleMess(const char * payload, size_t length) {
//  DynamicJsonBuffer jsonBuffer;
//  JsonObject& root = jsonBuffer.parseObject(payload);
//  String from = root["from"];
//  String mess = root["message"];
//  Serial.println(from);
//  Serial.println(mess);
//}

void Web_update_led(const char * payload, size_t length) {
  if (ledStatus) {
    webSocket.emit("Esp-update-led-to-web", "\"ON\"");
  } else {
    webSocket.emit("Esp-update-led-to-web", "\"OFF\"");
  }
}
void Web_update_fan(const char * payload, size_t length) {
  if (fanStatus) {
    webSocket.emit("Esp-update-fan-to-web", "\"ON\"");
  } else {
    webSocket.emit("Esp-update-fan-to-web", "\"OFF\"");
  }
}
void Turn_on_off_led(const char * payload, size_t length) {
  Serial.print("PayLoad: ");
  for (int i=0; i<=length; i++){
    Serial.print(payload[i]);
    Serial.print("    ");
  }
  Serial.println();
  if(payload[1] == 'N') {
    ledStatus = ON;
  } else {
    ledStatus = OFF;
  }
}
void Turn_on_off_fan(const char * payload, size_t length) {
  Serial.print("PayLoad: ");
  for (int i=0; i<=length; i++){
    Serial.print(payload[i]);
    Serial.print("    ");
  }
  Serial.println();
  if(payload[1] == 'N') {
    fanStatus = ON;
  } else {
    fanStatus = OFF;
  }
}
/*=======================================Setup===================================================*/
void setup() {
  
    Serial.begin(115200);
    EEPROM.begin(512);
    delay(100);
    ESP.wdtEnable(0);
    
    pinMode(buttonLed, INPUT);
    pinMode(buttonFan, INPUT);
    pinMode(Led, OUTPUT);
    pinMode(Fan, OUTPUT);
    
    valStatusled = EEPROM.read(0);
    valStatusfan = EEPROM.read(1);
    Serial.print("EEPROM_SETUP 0: ");
    Serial.println(EEPROM.read(0));
    Serial.print("EEPROM_SETUP 1: ");
    Serial.println(EEPROM.read(1));
    ledStatus = valStatusled;
    fanStatus = valStatusfan;
    digitalWrite(Led,valStatusled);
    digitalWrite(Fan,valStatusfan);

    beginTime = millis();
    WiFi.begin(ssid, pwdWifi);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(300);
      handlWifidisconnect();
      Serial.print(".");
      endTime = millis();
      if (endTime - beginTime > 600000)
      {
        ESP.restart();
      }
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
    
    beginTime = endTime = 0;
    
    webSocket.on("Req-update-led-from-web", Web_update_led);
    webSocket.on("Req-update-fan-from-web", Web_update_fan);
    webSocket.on("Turn-on-off-led-form-web", Turn_on_off_led);
    webSocket.on("Turn-on-off-fan-form-web", Turn_on_off_fan);
    webSocket.begin(Host_Socket, Port_Socket, "/socket.io/?transport=websocket");
    
    webSocket.emit("esp-join-room");
    
    digitalWrite(Led,valStatusled);
    digitalWrite(Fan,valStatusfan);

}
                       
/*============================Loop==============================================================*/
void loop() {
    if (webSocket.StatusConnectSocket==false)       /* nếu bị mất kết nối với socket thì reset lại */
    {
      ESP.restart();
    }
    checkStatus();
    checkButton();
    
    webSocket.loop();
}

/*--------------------Check Status---------------------*/
void checkStatus()
{
  checkStatus_led();
  checkStatus_fan();
}
void checkStatus_led()
{
  if (valStatusled != ledStatus)
  {
     digitalWrite(Led,ledStatus);
     valStatusled = ledStatus;
     EEPROM.write(0,valStatusled);
     EEPROM.commit();
  }
}
void checkStatus_fan()
{
  if (valStatusfan != fanStatus)
  {
     digitalWrite(Fan,fanStatus);
     valStatusfan = fanStatus;
     EEPROM.write(1,valStatusfan);
     EEPROM.commit();
  }
}
/*--------------------Check Button---------------------*/
void checkButton()
{
  checkButton_led();
  checkButton_fan();
}
void checkButton_led()
{
  if (!digitalRead(buttonLed))
  { 
     delay(500);
     ledStatus = !ledStatus;  
     if (ledStatus) 
     {
        webSocket.emit("Esp-update-led-to-web", "\"ON\"");
     } else {
        webSocket.emit("Esp-update-led-to-web", "\"OFF\"");
     }
  }
}
void checkButton_fan()
{
  if (!digitalRead(buttonFan))
  {
     delay(500);
     fanStatus = !fanStatus;
     if (fanStatus) 
     {
        webSocket.emit("Esp-update-fan-to-web", "\"ON\"");
     } else {
        webSocket.emit("Esp-update-fan-to-web", "\"OFF\"");
     }
  }
}
void handlWifidisconnect()
{
  checkStatus();
  if (!digitalRead(buttonLed))
  { 
     delay(500);
     ledStatus = !ledStatus;  
  }
  if (!digitalRead(buttonFan))
  {
     delay(500);
     fanStatus = !fanStatus;
  }
}
