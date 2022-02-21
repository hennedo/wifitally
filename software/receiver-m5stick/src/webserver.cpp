#include <webserver.h>
#include <settings.h>
#include <WiFi.h>
#include <html/onboarding.h>
#include <html/styles.h>
#include <html/index.h>
#include <version.h>
#include <battery_state.h>

String processor(const String& var) {
  if(var == "version")
    return F(VERSION);
  if(var == "hostname")
    return F(hostname);
  if(var == "id") {
    char id[2];
    itoa(settings.getID()+1, id, 10);
    return F(id);
  }
  if(var == "current_ip" && WiFi.getMode() == WIFI_MODE_AP)
    return F(WiFi.softAPIP().toString().c_str());
  if(var == "current_ip" && WiFi.getMode() == WIFI_MODE_STA)
    return F(WiFi.localIP().toString().c_str());
  if(var == "battery_percentage") {
    char percentage[4];
    itoa(getBatteryPercentage(), percentage, 10);
    return F(percentage);
  }
  if(var == "battery_voltage") {
    char voltage[8];
    dtostrf(getBatteryVoltage(), 3, 2, voltage);
    return F(voltage);
  }
  if(var == "useDHCP") {
      if(!settings.getUseDHCP()) {
          return F("");
      }
      return F("checked");
  }
  if(var == "enableBackLed") {
      if(!settings.getEnableLED()) {
          return F("");
      }
      return F("checked");
  }
  if(var == "showMiniOverview") {
      if(!settings.getEnableMiniOverview()) {
          return F("");
      }
      return F("checked");
  }
  if(var == "ssid") {
      return F(settings.getSSID());
  }
  if(var == "password") {
      return F(settings.getPassword());
  }
  if(var == "ip") {
      return F(settings.getIPAddress().toString().c_str());
  }
  if(var == "subnet") {
      return F(settings.getSubnet().toString().c_str());
  }
  if(var == "gateway") {
      return F(settings.getGateway().toString().c_str());
  }
  if(var == "brightness") {
    char brightness[3];
    itoa(settings.getBrightness(), brightness, 10);
    return F(brightness);
  }
  if(var == "ledBrightness") {
    char brightness[4];
    itoa(settings.getLEDBrightness(), brightness, 10);
    return F(brightness);
  }
  if(var == "network" && settings.getUseDHCP()) {
      return F("hidden");
  }
  if(var == "led" && !settings.getEnableLED()) {
      return F("hidden");
  }
  return String();
}

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", onboarding_html, processor); 
  }
};

void handleRoot(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
}

void serveCss(AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", styles_css);
}

void handleWifiConfig(AsyncWebServerRequest *request) {
    bool useDHCP = false;
    char ssid[33];
    char password[64];
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;
    if(request->hasParam("ssid", true)) {
        strcpy(ssid, request->getParam("ssid", true)->value().c_str());
    }
    if(request->hasParam("password", true)) {
        strcpy(password, request->getParam("password", true)->value().c_str());
    }
    if(request->hasParam("useDHCP", true) && request->getParam("useDHCP", true)->value().equals("true")) {
        useDHCP = true;
    }
    if(request->hasParam("ip", true)) {
        ip.fromString(request->getParam("ip", true)->value());
    }
    if(request->hasParam("gateway", true)) {
        gateway.fromString(request->getParam("gateway", true)->value());
    }
    if(request->hasParam("subnet", true)) {
        subnet.fromString(request->getParam("subnet", true)->value());
    }
    request->send(200, "text/html", "OK - restarting");
    if(useDHCP) {
        settings.setWiFiConfig(ssid, password);
    } else {
        settings.setWiFiConfig(ssid, password, ip, subnet, gateway);
    }
    ESP.restart();
}

void handleReset(AsyncWebServerRequest *request) {
    request->send(200, "text/html", "OK - restarting and resetting");
    settings.clearPreferences();
    ESP.restart();
}

void handleSettings(AsyncWebServerRequest *request) {
    bool showMiniOverview = false;
    bool enableBackLED = false;
    unsigned int id = settings.getID();
    unsigned int brightness = settings.getBrightness();
    unsigned int ledBrightness = settings.getLEDBrightness();
    if(request->hasParam("showMiniOverview", true) && request->getParam("showMiniOverview", true)->value().equals("true")) {
        showMiniOverview = true;
    }
    if(request->hasParam("enableBackLED", true) && request->getParam("enableBackLED", true)->value().equals("true")) {
        enableBackLED = true;
    }
    if(request->hasParam("brightness", true)) {
        brightness = atoi(request->getParam("brightness", true)->value().c_str());
    }
    if(request->hasParam("ledBrightness", true)) {
        ledBrightness = atoi(request->getParam("ledBrightness", true)->value().c_str());
    }
    if(request->hasParam("id", true)) {
        id = atoi(request->getParam("id", true)->value().c_str());
        settings.setID(id-1);
    }
    settings.setConfig(brightness, ledBrightness, enableBackLED, showMiniOverview);
    request->redirect("/");
}

void Webserver::begin() {
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    server.begin();
}