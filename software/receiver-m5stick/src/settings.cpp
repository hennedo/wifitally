#include <settings.h>

Preferences preferences;
Settings settings;

unsigned int Settings::getID() {
    return id;
}

char* Settings::getSSID() {
    return ssid;
}
char* Settings::getPassword() {
    return password;
}

bool Settings::getUseDHCP() {
    return useDHCP;
}

IPAddress Settings::getIPAddress() {
    return ipAddress;
}

IPAddress Settings::getSubnet() {
    return subnet;
}

IPAddress Settings::getGateway() {
    return gateway;
}

bool Settings::isIDWritten() {
    return id == writtenID;
}

bool Settings::getEnableMiniOverview() {
    return enableMiniOverview;
}

bool Settings::getEnableLED() {
    return enableLed;
}

unsigned int Settings::getBrightness() {
    return brightness;
}

unsigned int Settings::getLEDBrightness() {
    return ledBrightness;
}

void Settings::setEnableMiniOverview(bool v) {
    enableMiniOverview = v;
    preferences.putBool("enableMiniOverview", v);
}

void Settings::setEnableLED(bool v) {
    enableLed = v;
    preferences.putBool("enableLed", v);
}

void Settings::setID(unsigned int _id) {
    lastChange = millis();
    id = _id;
}

void Settings::setWiFiConfig(const char _ssid[33], const char _password[64]) {
    useDHCP = true;
    strcpy(ssid, _ssid);
    strcpy(password, _password);
    writeWifiConfig();
}

void Settings::setWiFiConfig(const char _ssid[33], const char _password[64], IPAddress _ip, IPAddress _subnet, IPAddress _gateway) {
    useDHCP = false;
    strcpy(ssid, _ssid);
    strcpy(password, _password);
    ipAddress = _ip;
    subnet = _subnet;
    gateway = _gateway;
    writeWifiConfig();
}

void Settings::setConfig(unsigned int _brightness, unsigned int _ledBrightness, bool _enableLed, bool _enableMiniOverview) {
    brightness = _brightness;
    ledBrightness = _ledBrightness;
    enableLed = _enableLed;
    enableMiniOverview = _enableMiniOverview;
    writeConfig();
}

void Settings::clearPreferences() {
    id = 0;
    writtenID = 0;
    lastChange = millis();
    strcpy(ssid, "");
    strcpy(password, "");
    ipAddress = (uint32_t)0;
    subnet = (uint32_t)0;
    gateway = (uint32_t)0;
    useDHCP = true;
    brightness = 15;
    ledBrightness = 255;
    enableLed = false;
    enableMiniOverview = true;
    preferences.clear();
}

void Settings::begin() {
    preferences.begin("wifitally");
    readAll();
}

void Settings::readAll() {
    id = preferences.getUInt("id", 0);
    brightness = preferences.getUInt("brightness", 15);
    ledBrightness = preferences.getUInt("ledBrightness", 255);
    writtenID = id;
    useDHCP = preferences.getBool("useDHCP", true);
    enableLed = preferences.getBool("enableLed", false);
    enableMiniOverview = preferences.getBool("enableMiniOverview", true);
    ipAddress.fromString(preferences.getString("ip", "0.0.0.0"));
    subnet.fromString(preferences.getString("subnet", "255.255.255.0"));
    gateway.fromString(preferences.getString("gateway", "0.0.0.0"));
    preferences.getString("ssid", ssid, 32);
    preferences.getString("password", password, 63);
}

// This should get called every loop in order to minimize wear for id saving
void Settings::update() {
    // if it did not change for 3 seconds... write it!
    if(lastChange+3000 <= millis() && writtenID != id) {
        writeID();
    }
}

void Settings::writeID() {
    Serial.println("writing id to storage");
    writtenID = id;
    preferences.putUInt("id", id);
}

void Settings::writeConfig() {
    if(writtenID != id) {
        writeID();
    }
    preferences.putBool("enableLed", enableLed);
    preferences.putBool("enableMiniOverview", enableMiniOverview);
    preferences.putUInt("brightness", brightness);
    preferences.putUInt("ledBrightness", ledBrightness);
}

void Settings::writeWifiConfig() {
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.putBool("useDHCP", useDHCP);
    preferences.putString("ip", ipAddress.toString());
    preferences.putString("subnet", subnet.toString());
    preferences.putString("gateway", gateway.toString());
}