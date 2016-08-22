#include <SocketIOClient.h>

#define LedPin 2
#define ButtonPin 0

#define SOFTAP_MODE

#ifdef SOFTAP_MODE
const char* password = "myMinion";
#else
const char* ssid = "SkyNet";
const char* password = "myMaster";
#endif

const char HexLookup[17] = "0123456789ABCDEF";

String host = "192.168.4.2";
int port = 3000;
bool clicked = false;

SocketIOClient socket;

void setupNetwork() {

  #ifdef SOFTAP_MODE
    WiFi.disconnect();
    byte mac[6];
    WiFi.macAddress(mac);
    char ssid[14] = "Minion-000000";
    ssid[7] = HexLookup[(mac[3] & 0xf0) >> 4];
    ssid[8] = HexLookup[(mac[3] & 0x0f)];
    ssid[9] = HexLookup[(mac[4] & 0xf0) >> 4];
    ssid[10] = HexLookup[(mac[4] & 0x0f)];
    ssid[11] = HexLookup[(mac[5] & 0xf0) >> 4];
    ssid[12] = HexLookup[(mac[5] & 0x0f)];
    ssid[13] = 0;
    WiFi.softAP(ssid, password);
  #else
    WiFi.begin(ssid, password);
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
    if(i == 21){
      while(1) delay(500);
    }
  #endif

}

void click() {
  clicked = true;
}

void light(String state) {
  Serial.println("[light] " + state);
  if (state == "\"state\":true") {
    Serial.println("[light] ON");
    digitalWrite(LedPin, HIGH);
  }
  else {
    Serial.println("[light] off");
    digitalWrite(LedPin, LOW);
  }
}

//
// This code runs only once
//
void setup() {

  // set up our pins
  pinMode(LedPin, OUTPUT);
  pinMode(ButtonPin, INPUT);

  digitalWrite(LedPin, LOW);

  Serial.begin(115200);

  setupNetwork();

  attachInterrupt(digitalPinToInterrupt(ButtonPin), click, FALLING);    

  socket.on("light", light);

  socket.connect(host, port);
}

void clickCheck() {
  if (clicked) {
    Serial.println("[click]");
    socket.emit("toggle", "{\"state\":true}");
    clicked = false;
  }  
}

//
// This code runs over and over again
//
void loop() {
  socket.monitor();
  clickCheck();
}


