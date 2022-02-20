#include<settings.h>

Preferences preferences;
Settings settings;

void Settings::setID(unsigned int _id) {
    lastChange = millis();
    id = _id;
}
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
    preferences.clear();
}

void Settings::begin() {
    preferences.begin("wifitally");
    readAll();
}

void Settings::readAll() {
    id = preferences.getUInt("id", 0);
    writtenID = id;
    useDHCP = preferences.getBool("useDHCP", true);
    ipAddress.fromString(preferences.getString("ip", "0.0.0.0"));
    subnet.fromString(preferences.getString("subnet", "255.255.255.0"));
    gateway.fromString(preferences.getString("gateway", "0.0.0.0"));
    preferences.getString("ssid", ssid, 32);
    preferences.getString("password", password, 63);


}

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

void Settings::writeWifiConfig() {
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.putBool("useDHCP", useDHCP);
    preferences.putString("ip", ipAddress.toString());
    preferences.putString("subnet", subnet.toString());
    preferences.putString("gateway", gateway.toString());
}