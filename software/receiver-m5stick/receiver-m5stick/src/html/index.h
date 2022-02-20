#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>WifiTally Config</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>WifiTally Config Page</h3>
  <br><br>
  <form action="/get">
    <br>
    Name: <input type="text" name="name">
    <br>
    ESP32 Proficiency: 
    <select name = "proficiency">
      <option value=Beginner>Beginner</option>
      <option value=Advanced>Advanced</option>
      <option value=Pro>Pro</option>
    </select>
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";