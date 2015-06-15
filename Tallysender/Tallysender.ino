#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0xF2, 0x00
};      // <= SETUP!
//IPAddress ip(192, 168, 8, 200);              // <= SETUP!
IPAddress server(224,0,0,20);             // <= MULTICAST
//IPAddress server(192, 168, 8, 104);             // <= UNICAST
#include <ATEM.h>

ATEM AtemSwitcher;
EthernetClient client;
EthernetUDP udp;

void setup() {
  Ethernet.begin(mac);
  Serial.begin(115200);
  Serial.println("Serial started");

  //AtemSwitcher.begin(IPAddress(192, 168, 8, 240), 56417);    // <= SETUP!
  //AtemSwitcher.connect();
  udp.beginMulti(server, 3000);
}

bool AtemOnline = false;

bool preview = false;
bool live = false;
void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  //AtemSwitcher.runLoop();

  // If connection is gone anyway, try to reconnect:
  //if (AtemSwitcher.isConnectionTimedOut())  {
  //  Serial.println("Connection to ATEM Switcher has timed out - reconnecting!");
  //  AtemSwitcher.connect();
  //}
  //setTallys();
  sendChar("010000000");
  delay(1000);
  sendChar("20000000");
  delay(1000);
  sendChar("12000000");
  delay(1000);
  delay(20);
  Serial.println("send");
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
  Serial.println(tallys);
  sendChar(tallys);
}

void sendChar(char packet[8]) {
  udp.beginPacket(server, 3000);
  udp.write(packet, 8);
  udp.endPacket();  
}

