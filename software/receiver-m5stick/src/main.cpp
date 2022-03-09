#include <main.h>

M5GFX display;
M5Canvas canvas(&display);
DNSServer dnsServer;
IPAddress multicastAddress(224, 0, 0, 20);
Webserver webserver;
bool isConnected = false;
unsigned int oldBrightness = 15;
unsigned long lastBatteryCheck = 0;
char hostname[23];

void beginWiFi()
{
  char *ssid = settings.getSSID();
  if ((ssid != NULL) && (ssid[0] == '\0'))
  {
    // Start config AP;
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(hostname, NULL);
    WiFi.setHostname(hostname);
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", WiFi.softAPIP());
  }
  else
  {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.setHostname(hostname);
    if (!settings.getUseDHCP())
      WiFi.config(settings.getIPAddress(), settings.getGateway(), settings.getSubnet());
    WiFi.begin(settings.getSSID(), settings.getPassword());
  }
}

void setup()
{
  pinMode(M5_LED, OUTPUT);
  Serial.begin(115200);
  M5.begin();
  uint64_t chipid = ESP.getEfuseMac();
  snprintf(hostname, 23, "Wifitally-%08X", (uint32_t)chipid);
  settings.begin();
  display.begin();
  display.setRotation(1);
  canvas.createSprite(display.width(), display.height());
  beginWiFi();
  tallyReceiver.beginMulticast(multicastAddress, 3000);
  webserver.begin();
}

void loop()
{
  settings.update();
  M5.update();
  if (lastBatteryCheck + 100 < millis())
    calcBatteryPercentage(M5.Axp.GetBatVoltage());
  if (settings.getBrightness() != oldBrightness)
  {
    oldBrightness = settings.getBrightness();
    Serial.printf("Updating Brightness to %d\n", oldBrightness);
    M5.Axp.ScreenBreath(oldBrightness);
  }
  if (millis() < 6000 && M5.BtnB.pressedFor(5000))
  {
    Serial.println("resetting");
    settings.clearPreferences();
    ESP.restart();
  }

  int noBytes = tallyReceiver.parsePacket();
  if (noBytes)
    readData(noBytes);

  // Set Wifi State...
  if (WiFi.getMode() == WIFI_MODE_AP)
  {
    state = StateOnboarding;
    dnsServer.processNextRequest();
  }
  if (WiFi.getMode() == WIFI_MODE_STA && !WiFi.isConnected())
  {
    isConnected = false;
    state = StateConnecting;
  }
  // Once we are connected, show tally screen
  if (WiFi.getMode() == WIFI_MODE_STA && WiFi.isConnected() && !isConnected)
  {
    tallyReceiver.beginMulticast(multicastAddress, 3000);
    isConnected = true;
    state = StateShowTally;
  }
  
  if(settings.getEnableLED() && packetBuffer[settings.getID()] == '2')
    digitalWrite(M5_LED, LOW);
  else
    digitalWrite(M5_LED, HIGH);

  if (state == StateShowTally && M5.BtnA.wasPressed())
    incID();
  if (state == StateShowTally && M5.BtnB.wasPressed())
    state = StateShowMenu;

  switch (state)
  {
  case StateBooting:
    bootingScreen().pushSprite(&display, 0, 0);
    break;

  case StateShowTally:
    tallyScreen(packetBuffer).pushSprite(&display, 0, 0);
    break;

  case StateConnecting:
    connectingScreen().pushSprite(&display, 0, 0);
    break;

  case StateOnboarding:
    onboardingScreen(hostname).pushSprite(&display, 0, 0);
    break;
  case StateShowMenu:
    menuScreen().pushSprite(&display, 0, 0);
    break;
  default:
    break;
  }
}

void readData(int noBytes)
{
  Serial.println("New Data!");
  tallyReceiver.read(packetBuffer, noBytes);
}

void incID()
{
  settings.setID((settings.getID() + 1) % 8);
}