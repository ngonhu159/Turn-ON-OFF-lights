#include <ArduinoJson.h>

#include <ESP8266WiFi.h>

#include <SocketIoClient.h>

#include <EEPROM.h>

SocketIoClient webSocket;

const char* Host_Socket = "ngoquocnhu.herokuapp.com";

//const char* Host_Socket = "192.168.137.1";
//unsigned int Port_Socket = 3000;
unsigned int Port_Socket=80;
const char* ssid = "Wifi";
const char* pwdWifi = "29081999";
                                 
/*==========================================================================================*/
#define ON 1
#define OFF 0

/*==========================================================================================*/
byte ledStatus = OFF;
int Val_eeprom;
char temp = 1;                              
/*============================================ Event Socket IO ==============================================*/

void handleMess(const char * payload, size_t length) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  String from = root["from"];
  String mess = root["message"];
  Serial.println(from);
  Serial.println(mess);
}

void joinSuccess(const char * payload, size_t length) {
  webSocket.emit("esp-send-server-update-led","\"updated\"");
}

void togleLed(const char * payload, size_t length) {
  ledStatus = !ledStatus;
}

void yeuCauCapNhat(const char * payload, size_t length) {
  if (ledStatus) {
    webSocket.emit("esp-cap-nhat-nhat-trang-thai", "\"ON\"");
  } else {
    webSocket.emit("esp-cap-nhat-nhat-trang-thai", "\"OFF\"");
  }
}

void capNhatLed(const char * payload, size_t length) {
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
                               
/*=======================================Setup===================================================*/
void setup() {
  
    Serial.begin(115200);
    Serial.println();
    EEPROM.begin(512);
    delay(100);
    ESP.wdtEnable(0);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    
    Val_eeprom = EEPROM.read(0);
    Serial.print("EEPROM_SETUP: ");
    Serial.println(EEPROM.read(0));
    Serial.print("Val_eeprom: ");
    Serial.println(Val_eeprom);
    digitalWrite(D5,!Val_eeprom);
    digitalWrite(D6,!Val_eeprom);
    WiFi.begin(ssid, pwdWifi);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
  
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
    
    webSocket.on("server-send-join-success", joinSuccess);
    webSocket.on("server-send-togle", togleLed);
    webSocket.on("yeu-cau-cap-nhat-trang-thai", yeuCauCapNhat);
    webSocket.on("cap-nhat-led", capNhatLed);
    webSocket.begin(Host_Socket, Port_Socket, "/socket.io/?transport=websocket");
    webSocket.emit("esp-send-join-room");
    
    digitalWrite(D5,!Val_eeprom);
    digitalWrite(D6,!Val_eeprom);
    
}
                       
/*============================Loop==============================================================*/
void loop() {

    if (webSocket.StatusConnectSocket==false)       /* nếu bị mất kết nối với socket thì reset lại */
    {
      Serial.println("123456789NHU");
      while(1);
    }
    
    if (Val_eeprom != ledStatus && temp == 2)
    {
      digitalWrite(D5,!ledStatus);
      digitalWrite(D6,!ledStatus);
      Val_eeprom = ledStatus;
      EEPROM.write(0,Val_eeprom);
      EEPROM.commit();
      Serial.print("LedStatus: ");
      Serial.println(ledStatus);
      Serial.print("Val_eeprom ");
      Serial.println(Val_eeprom);
      Serial.print("EEPROM: ");
      Serial.println(EEPROM.read(0));
    }
    if (temp == 1)
    {
      temp = 2;
      ledStatus = Val_eeprom;
    }
    
    webSocket.loop();
}
