#include <Arduino.h>
#include <ATEM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// Setup

byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x00}; // mac address of your ethernet shield
IPAddress      ip(192, 168, 1  , 55 );             // local ip
IPAddress gateway(192, 168, 1  , 1  );             // local gateway
IPAddress  subnet(255, 255, 255, 0  );             // subnet
IPAddress  atemIP(192, 168, 1  , 2  );             // ip of the atem


IPAddress  server(224, 0  , 0  , 20 );             // Multicast ip to send to
char tallys_old[8];
unsigned long time_old, lastDisplay;
ATEM AtemSwitcher;
EthernetUDP udp;


void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip, gateway, subnet);
  delay(1000);
  Serial.println(Ethernet.localIP());
  time_old = millis();
  AtemSwitcher.begin(atemIP, 56417);
  AtemSwitcher.connect();
  udp.beginMulticast(server, 3000);
}

void sendChar(char packet[8]) {
  udp.beginPacket(server, 3000);
  udp.write(packet, 8);
  udp.endPacket();  
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
  if(strcmp(tallys_old, tallys) != 0 || millis() > (time_old+1000) ) {
    Serial.print("Old: [");
    Serial.print(tallys_old);
    Serial.println("]");
    Serial.print("New: [");
    Serial.print(tallys);
    Serial.println("]");
    sendChar(tallys);
    time_old = millis();
  }
  memcpy(tallys_old, tallys, 8);
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  if (AtemSwitcher.isConnectionTimedOut())  {
    Serial.println("Connection to ATEM Switcher has timed out - reconnecting!");
    AtemSwitcher.connect();
  }
  if(AtemSwitcher.isConnected()) {
    setTallys();
  }
  delay(50);
}