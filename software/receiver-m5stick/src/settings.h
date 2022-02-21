#include <WiFi.h>
#include <Preferences.h>

class Settings {
    public:
        // Setters
        void setID(unsigned int);
        void setWiFiConfig(const char ssid[33], const char password[64]);
        void setWiFiConfig(const char ssid[33], const char password[64], IPAddress ip, IPAddress subnet, IPAddress gateway);
        void setConfig(unsigned int brightness, unsigned int ledBrightness, bool enableLed, bool enableMiniOverview);
        void clearPreferences();
        void setEnableLED(bool);
        void setEnableMiniOverview(bool);

        // Getters
        unsigned int getID();
        char* getSSID();
        char* getPassword();
        bool getUseDHCP();
        unsigned int getBrightness();
        unsigned int getLEDBrightness();
        bool getEnableLED();
        bool getEnableMiniOverview();
        IPAddress getIPAddress();
        IPAddress getSubnet();
        IPAddress getGateway();
        bool isIDWritten();

        // Lifecycle stuff
        void update();
        void begin();

    private:

        void writeID();
        void writeConfig();
        void writeWifiConfig();
        void readAll();

        // WiFi Settings
        char ssid[33];
        char password[64];
        bool useDHCP;
        IPAddress ipAddress;
        IPAddress subnet;
        IPAddress gateway;

        // Tally Stuff
        unsigned int id;
        unsigned int brightness;
        unsigned int ledBrightness;
        bool enableLed;
        bool enableMiniOverview;

        // internal stuff
        unsigned int writtenID;
        unsigned long lastChange;
};


extern Settings settings;