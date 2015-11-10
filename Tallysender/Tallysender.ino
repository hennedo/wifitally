#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x00
};      // <= SETUP!
IPAddress ip(192, 168, 10, 200);              // <= SETUP!
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);
IPAddress server(224,0,0,20);             // <= MULTICAST
//IPAddress server(192, 168, 8, 104);             // <= UNICAST
#include <ATEM.h>

char tallys_old[8];
unsigned long time_old;
ATEM AtemSwitcher;
EthernetClient client;
EthernetUDP udp;

void setup() {
  Ethernet.begin(mac, ip, gateway, subnet);
  Serial.println(Ethernet.localIP());
  Serial.begin(115200);
  Serial.println("Serial started");
  time_old = millis();
  AtemSwitcher.begin(IPAddress(192, 168, 10, 240), 56417);    // <= SETUP!
  AtemSwitcher.connect();
  udp.beginMulti(server, 3000);
}

bool AtemOnline = false;

bool preview = false;
bool live = false;
void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    Serial.println("Connection to ATEM Switcher has timed out - reconnecting!");
    AtemSwitcher.connect();
  }
  setTallys();
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

