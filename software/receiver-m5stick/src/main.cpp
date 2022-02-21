#include <main.h>

M5GFX display;
M5Canvas canvas(&display);
DNSServer dnsServer;
IPAddress multicastAddress(224,0,0,20);
Webserver webserver;
enum State { StateBooting, StateOnboarding, StateConnecting, StateShowTally, StateShowMenu};
State state = StateBooting;
bool isConnected = false;
unsigned int oldBrightness = 15;
unsigned long lastBatteryCheck = 0;
char hostname[23];
unsigned int menuItem = 0;

void beginWiFi() {
  char *ssid = settings.getSSID();
  if ((ssid != NULL) && (ssid[0] == '\0')) {
    // Start config AP;
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(hostname, NULL);
    WiFi.setHostname(hostname);
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", WiFi.softAPIP());
  } else {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.setHostname(hostname);
    if(!settings.getUseDHCP()) {
      WiFi.config(settings.getIPAddress(), settings.getGateway(), settings.getSubnet());
    }
    WiFi.begin(settings.getSSID(), settings.getPassword());
  }

}

void setup() {
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

void loop() {
  settings.update();
  M5.update();
  if(lastBatteryCheck+100 < millis()) {
    calcBatteryPercentage(M5.Axp.GetBatVoltage());
  }
  if(settings.getBrightness() != oldBrightness) {
    oldBrightness = settings.getBrightness();
    Serial.printf("Updating Brightness to %d\n", oldBrightness);
    M5.Axp.ScreenBreath(oldBrightness);
  }
  if(millis() < 6000 && M5.BtnB.pressedFor(5000)) {
    Serial.println("resetting");
    settings.clearPreferences();
    ESP.restart();
  }


  int noBytes = tallyReceiver.parsePacket();
  if(noBytes) {
    readData(noBytes); 
  }

  // Set Wifi State... 
  if(WiFi.getMode() == WIFI_MODE_AP) {
    state = StateOnboarding;
    dnsServer.processNextRequest();
  }
  if(WiFi.getMode() == WIFI_MODE_STA && !WiFi.isConnected()) {
    isConnected = false;
    state = StateConnecting;
  }
  // Once we are connected, show tally screen
  if(WiFi.getMode() == WIFI_MODE_STA && WiFi.isConnected() && !isConnected) {
    tallyReceiver.beginMulticast(multicastAddress, 3000);
    isConnected = true;
    state = StateShowTally;
  }


  if(state == StateShowTally && M5.BtnA.wasPressed()) {
    incID();
  }
  if(state == StateShowMenu && M5.BtnB.wasPressed()) {
    menuItem = (menuItem+1)%3;
  }
  if(state == StateShowTally && M5.BtnB.wasPressed()) {
    menuItem = 0;
    state = StateShowMenu;
  }
  if(state == StateShowMenu && M5.BtnA.wasPressed()) {
    switch (menuItem)
    {
    case 0:
      settings.setEnableMiniOverview(!settings.getEnableMiniOverview());
      break;
    case 1:
      settings.setEnableLED(!settings.getEnableLED());
      break;
    case 2:
      state = StateShowTally;
      break;
    default:
      break;
    }
  }

  switch (state)
  {
  case StateBooting:
    showBooting();
    break;
  
  case StateShowTally:
    showTally();
    break;
  
  case StateConnecting:
    showConnecting();
    break;
  
  case StateOnboarding:
    showOnboarding();
    break;
  case StateShowMenu:
    showMenu();
    break;
  default:
    break;
  }
}


void readData(int noBytes) {
  Serial.println("New Data!");
  tallyReceiver.read(packetBuffer,noBytes);
  if(noBytes != 8) {
   return; 
  }
}

void incID() {
  settings.setID((settings.getID()+1)%8);
  Serial.print("incing id: ");
  Serial.println(settings.getID());
}

void showConnecting() {
  if(state != StateConnecting) return;
  canvas.fillScreen(TFT_BLACK);
  canvas.setTextSize(2);
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  canvas.setCursor(5,30);
  canvas.print("Connecting");
  int dotCount = (millis()%1000)/334;
  for(int i=0; i < dotCount; i++) {
    canvas.print(".");
  }
  canvas.setTextSize(1);
  canvas.setCursor(5,50);
  canvas.printf("To SSID: %s", settings.getSSID());
  canvas.pushSprite(&display, 0, 0);
}

void showBooting() {
  if(state != StateBooting) return;
  canvas.fillScreen(TFT_BLACK);
  canvas.setTextSize(2);
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  canvas.setCursor(5,30);
  canvas.print("Booting");
  int dotCount = millis()/300%3;
  for(int i=0; i< dotCount; i++) {
    canvas.print(".");
  }
  canvas.pushSprite(&display, 0, 0);
}

void showOnboarding() {
  if(state != StateOnboarding) return;
  canvas.fillScreen(TFT_BLACK);
  canvas.setTextSize(2);
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  canvas.setCursor(5,10);
  canvas.print("Onboarding");
  canvas.setTextSize(1);
  canvas.setCursor(5,28);
  canvas.print("Connect to SSID:");
  canvas.setCursor(5,37);
  canvas.setTextColor(TFT_RED);
  canvas.print(hostname);
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  canvas.setCursor(5,46);
  canvas.print("and go to:");
  canvas.setCursor(5,55);
  canvas.setTextColor(TFT_RED);
  canvas.print("http://");
  WiFi.softAPIP().printTo(canvas);
  canvas.pushSprite(&display, 0, 0);
}

void showTally() {
  if(state != StateShowTally) return;
  canvas.fillScreen(TFT_BLACK);
  // Set own tally color
  int ownColor = TFT_BLACK;
  if(packetBuffer[settings.getID()] == '1') ownColor = TFT_GREEN;
  if(packetBuffer[settings.getID()] == '2') ownColor = TFT_RED;

  if(settings.getEnableMiniOverview()) {
    canvas.drawRect(73, 3, 83, 74, TFT_DARKGREY);
    canvas.fillRect(74, 4, 81, 72, ownColor);
  } else {
    canvas.drawRect(34, 3, 122, 74, TFT_DARKGREY);
    canvas.fillRect(35, 4, 120, 72, ownColor);
  }

  canvas.setTextSize(4);
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  canvas.setCursor(10,25);
  if(settings.isIDWritten()) {
    canvas.print(settings.getID()+1);
  } else {
    if(millis()%1000 < 500) {
      canvas.print(settings.getID()+1);
    } else {
      canvas.fillRect(0, 0, 34, 80, TFT_BLACK);
    }
  }
  if(settings.getEnableMiniOverview()) {
    canvas.setTextSize(1);
    int thumbSize = 15;
    int spacing = 4;
    for(int i=0; i<2; i++) {
      for(int j=0;j<4;j++) {
        int index = i+(2*j);
        int col = TFT_BLACK;
        int x = 35 + ((thumbSize+spacing)*i);
        int y = spacing + (thumbSize+spacing)*(j);
        if(packetBuffer[index] == '1') col = TFT_GREEN;
        if(packetBuffer[index] == '2') col = TFT_RED;
        if(settings.getID() == index) {
          canvas.drawRect(x-1, y-1, thumbSize+2, thumbSize+2, TFT_YELLOW);
        } else {
          canvas.drawRect(x-1, y-1, thumbSize+2, thumbSize+2, TFT_DARKGREY);
        }
        canvas.fillRect(x, y, thumbSize, thumbSize, col);
        canvas.setCursor(x+5, y+4);
        canvas.setTextColor(TFT_WHITE, col);
        canvas.print(index+1);
      }
    }
  }
  canvas.pushSprite(&display, 0, 0);
}

void batteryIcon(M5Canvas canvas, int32_t x, int32_t y) {
  canvas.setCursor(x, y);
  canvas.printf("%d%%", getBatteryPercentage());
  x += 24;
  canvas.drawRect(x, y, 25, 7, TFT_DARKGREY);
  int blocks = getBatteryPercentage()/34+1;
  for(int i=0; i < blocks; i++) {
    canvas.fillRect(x+1+(i*8), y+1, 7, 5, TFT_GREEN);
    if(i < 2) {
      canvas.drawLine(x+1+(i*8)+7, y+1, x+1+(i*8)+7, y+6, TFT_LIGHTGRAY);
    }
  }
}

void showMenu() {
  if(state != StateShowMenu) return;
  const int backColor = TFT_DARKCYAN;
  canvas.fillScreen(TFT_BLACK);
  canvas.fillRect(0,0,160, 11, TFT_WHITE);
  canvas.setTextColor(TFT_BLACK, TFT_WHITE);
  batteryIcon(canvas, 110, 2);
  canvas.setCursor(3, 2);
  canvas.setTextSize(1);
  canvas.print("Settings");
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  canvas.setCursor(3, 13);
  canvas.printf("ID: %d", settings.getID()+1);
  canvas.setCursor(3, 25);
  canvas.printf("IP: %s", WiFi.localIP().toString().c_str());
  canvas.setCursor(3, 37);
  if(menuItem == 0) {
    canvas.setTextColor(TFT_WHITE, backColor);
    canvas.fillRect(0, 35, 160, 11, backColor);
  }
  canvas.printf("Mini Icons: %s", settings.getEnableMiniOverview() ? "yes" : "no");
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  if(menuItem == 1) {
    canvas.setTextColor(TFT_WHITE, backColor);
    canvas.fillRect(0, 47, 160, 11, backColor);
  }
  canvas.setCursor(3, 49);
  canvas.printf("Back LED: %s", settings.getEnableLED() ? "yes" : "no");
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  if(menuItem == 2) {
    canvas.setTextColor(TFT_WHITE, backColor);
    canvas.fillRect(0, 64, 160, 11, backColor);
  }
  canvas.setCursor(3, 66);
  canvas.printf("Exit");

  canvas.pushSprite(&display, 0, 0);
}