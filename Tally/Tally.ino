#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
 
const char* ssid = "Drunken Records on tour";
const char* password = "recordsmobile";
IPAddress multicastAddress (224, 0, 0, 20);

WiFiUDP udp;
 
const int led = 13;
const int led2 = 12;
int id = 0; // 0-7 eigentlich einstellbar ueber dip switch.

byte packetBuffer[512];

void preview() {
  digitalWrite(led, 1);
  digitalWrite(led2, 0);
}

void live() {
  digitalWrite(led, 0);
  digitalWrite(led2, 1);
}
void off() {
  digitalWrite(led, 0);
  digitalWrite(led2, 0);
}
void connect() {
  
  Serial.println("Connecting");
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, 1);
    digitalWrite(led2, 1);
    delay(500);
    digitalWrite(led, 0);
    digitalWrite(led2, 0);
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  udp.beginMulticast(WiFi.localIP(), multicastAddress, 3000);
  
}

void readData(int noBytes) {
  udp.read(packetBuffer,noBytes);
  if(noBytes != 8) {
   return; 
  }
  char mine = char(packetBuffer[id]);
  if(mine=='0') {
   off();
  } else if(mine=='1') {
   preview(); 
  } else if(mine=='2') {
   live(); 
  }
}
void setup(void)
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led, 0);
  digitalWrite(led2, 0);
  
  connect();
}
 
void loop(void)
{
  if(WiFi.status() != WL_CONNECTED) {
    connect();
  }
  int noBytes = udp.parsePacket();
  if(noBytes) {
    readData(noBytes); 
  }

} 

