#include <ESPAsyncWebServer.h>

class Webserver
{
public:
    Webserver() : server(80) {}
    void begin();

private:
    AsyncWebServer server;
};