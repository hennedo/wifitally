#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <ATEM.h>

// Setup

byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x00}; // mac address of your ethernet shield
IPAddress ip(192, 168, 10, 200);          // local ip
IPAddress gateway(192,168,10,1);          // local gateway
IPAddress subnet(255,255,255,0);          // subnet
IPAddress server(224,0,0,20);             // Multicast ip to send to
IPAddress atemIP(192, 168, 10, 240);     // ip of the atem
// stop editing here, unless you know what you are doing or you want to use the display

// for display use, uncomment from here
//#include <Adafruit_GFX.h>
//#include <gfxfont.h>
//#include <Adafruit_SSD1306.h>
//#include "Wire.h"
//#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_RESET);
// to here
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
  // for display use, uncomment from here
//  Wire.begin();
//  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//  displayStatus();
  // to here
  time_old = millis();
  AtemSwitcher.begin(atemIP, 56417);    // <= SETUP!
  AtemSwitcher.connect();
  udp.beginMulticast(server, 3000);
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
// for display use, uncomment from here
//    int noBytes = udp.parsePacket();
//    if(noBytes) {
//      readData(noBytes); 
//    }
//    if((millis()-lastDisplay) > 3000) {
//      displayStatus();
//    }
// to here
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
// for display use, uncomment from here
//void displayStatus() {
//  display.clearDisplay();
//  display.setCursor(0,0);
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.println("TALLY");
//  display.println("TRANSMITTER");
//  display.println("v0.5");
//  display.setTextSize(1);
//  display.println(Ethernet.localIP());
//  display.display();
//  lastDisplay = millis();
//}
//
//void readData(int noBytes) {
// byte packetBuffer[512];
// udp.read(packetBuffer,noBytes);
// if(char(packetBuffer[0]) == 'l') {
//  int id = (char(packetBuffer[1]) - '0');
//  Serial.println("Low battery on tally " + String(id));
//  display.clearDisplay();
//  display.setCursor(0,0);
//  display.setTextSize(2);
//  display.println();
//  display.println("Tally " + String(char(packetBuffer[1])));
//  display.println("LOW");
//  display.display();
//  lastDisplay = millis();
//  
// } 
// if(char(packetBuffer[0]) == 's') {
//  int id = (char(packetBuffer[1]) - '0');
//  Serial.println("Tally " + String(id) + " shutting down");
//  display.clearDisplay();
//  display.setCursor(0,0);
//  display.setTextSize(2);
//  display.println();
//  display.println("Tally " + String(char(packetBuffer[1])));
//  display.println("DRAINED");
//  display.setTextSize(1);
//  display.println("Tally shut down");
//  display.display();
//  lastDisplay = millis();
//  
// } 
//}
// to here
