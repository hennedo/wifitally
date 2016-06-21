#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet2.h>
#include <Adafruit_GFX.h>
#include <ATEM.h>
#include <gfxfont.h>
#include <EthernetUdp2.h>
#include <Adafruit_SSD1306.h>
#include "Wire.h"


#define OLED_RESET 4

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x00
};      // <= SETUP!
IPAddress ip(192, 168, 10, 200);          // local ip
IPAddress gateway(192,168,10,1);          // local gateway
IPAddress subnet(255,255,255,0);          // subnet
IPAddress server(224,0,0,20);             // Multicast ip to send to
IPAddress atemIP(192, 168, 10, 240);     // ip of the atem
Adafruit_SSD1306 display(OLED_RESET);

char tallys_old[8];
unsigned long time_old, lastDisplay;
ATEM AtemSwitcher;
EthernetUDP udp;


bool AtemOnline = false;
bool preview = false;
bool live = false;

void setup() {
  Ethernet.begin(mac, ip, gateway, subnet);
  Serial.println(Ethernet.localIP());
  Serial.begin(9600);
  Serial.println("Serial started");
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  displayStatus();
  time_old = millis();
  AtemSwitcher.begin(atemIP, 56417);    // <= SETUP!
  AtemSwitcher.connect();
  udp.beginMulti(server, 3000);
}

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    Serial.println("Connection to ATEM Switcher has timed out - reconnecting!");
    AtemSwitcher.connect();
  }
  setTallys();
  int noBytes = udp.parsePacket();
  if(noBytes) {
    readData(noBytes); 
  }
  if((millis()-lastDisplay) > 3000) {
    displayStatus();
  }
}

void setTallys() {
  char tallys[8];
  for (uint8_t i = 1; i <= 8; i++) {
    if (AtemSwitcher.getProgramTally(i)) {
      tallys[i-1]='2';
    } else if (AtemSwitcher.getPreviewTally(i)) {
      tallys[i-1]='1';
    } else {
      tallys[i-1]='0';
    }
  }
  if(strcmp(tallys_old, tallys) || millis() > (time_old+1000) ) { 
    sendChar(tallys);
    time_old = millis();
  }
  strcpy(tallys_old, tallys);
}

void sendChar(char packet[8]) {
  udp.beginPacket(server, 3000);
  udp.write(packet, 8);
  udp.endPacket();  
}

void displayStatus() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("TALLY");
  display.println("TRANSMITTER");
  display.println("v0.5");
  display.setTextSize(1);
  display.println(Ethernet.localIP());
  display.display();
  lastDisplay = millis();
}

void readData(int noBytes) {
 byte packetBuffer[512];
 udp.read(packetBuffer,noBytes);
 if(char(packetBuffer[0]) == 'l') {
  int id = (char(packetBuffer[1]) - '0');
  Serial.println("Low battery on tally " + String(id));
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println();
  display.println("Tally " + String(char(packetBuffer[1])));
  display.println("LOW");
  display.display();
  lastDisplay = millis();
  
 } 
 if(char(packetBuffer[0]) == 's') {
  int id = (char(packetBuffer[1]) - '0');
  Serial.println("Tally " + String(id) + " shutting down");
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println();
  display.println("Tally " + String(char(packetBuffer[1])));
  display.println("DRAINED");
  display.setTextSize(1);
  display.println("Tally shut down");
  display.display();
  lastDisplay = millis();
  
 } 
}
