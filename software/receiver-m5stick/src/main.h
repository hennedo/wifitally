#include <M5StickC.h>
#include <M5GFX.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <settings.h>
#include <webserver.h>
#include <DNSServer.h>
#include <battery_state.h>
#include <version.h>

WiFiUDP tallyReceiver;
byte packetBuffer[512];
uint8_t chipid[6];

void off();
void preview();
void live();
void readData(int);
void incID();
void showTally();
void showConnecting();
void showBooting();
void showOnboarding();
void showMenu();