#include <ESPAsyncWebServer.h>

const char redirect_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><head>
    <title>WiFiTally</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="Refresh" content="0; url=/onboarding" />
    </head><body></body></html>)rawliteral";