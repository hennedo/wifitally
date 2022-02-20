#include <WiFi.h>
#include <Preferences.h>

class Settings {
    public:
        void setID(unsigned int);
        unsigned int getID();
        void setWiFiConfig(const char ssid[33], const char password[64]);
        void setWiFiConfig(const char ssid[33], const char password[64], IPAddress ip, IPAddress subnet, IPAddress gateway);
        char* getSSID();
        char* getPassword();
        bool getUseDHCP();
        IPAddress getIPAddress();
        IPAddress getSubnet();
        IPAddress getGateway();
        void clearPreferences();
        void update();
        void begin();
        bool isIDWritten();

    private:

        void writeID();
        void writeWifiConfig();
        void readAll();

        // WiFi Settings
        char ssid[33];
        char password[64];
        bool useDHCP;
        IPAddress ipAddress;
        IPAddress subnet;
        IPAddress gateway;

        // Tally ID
        unsigned int id;

        // internal stuff
        unsigned int writtenID;
        unsigned long lastChange;
};


extern Settings settings;