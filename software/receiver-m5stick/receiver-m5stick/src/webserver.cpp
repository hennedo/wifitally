#include <webserver.h>
#include <settings.h>
#include <html/onboarding.h>
#include <html/styles.h>
#include <html/index.h>


String processor(const String& var) {
  if(var == "This function currently does nothing.")
    return F("Hello world!");
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
    request->send_P(200, "text/html", onboarding_html); 
  }
};

void handleRoot(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
}

void serveCss(AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", styles_css, processor);
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
    request->send(200, "text/html", "OK");
    if(useDHCP) {
        settings.setWiFiConfig(ssid, password);
    } else {
        settings.setWiFiConfig(ssid, password, ip, subnet, gateway);
    }
    ESP.restart();
}

void Webserver::begin() {
    server.on("/styles.css", HTTP_GET, serveCss);
    server.on("/configure", HTTP_POST, handleWifiConfig);
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    server.on("/", HTTP_GET, handleRoot);
    server.begin();
}