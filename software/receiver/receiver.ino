#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// CONFIG
String defaultSSID = "Wifitally";
String defaultPassword = "wifitally2342";


// Stop editing here, unless you know what you are doing

const int ledR = 4,
          ledB = 5,
          ledG = 12,
          taster = 14;
int printes = 0;
bool printed = false;
IPAddress multicastAddress(224,0,0,20);
int pressed = 0;
bool changes = false;
bool started = false;
bool standalone = false;
String apssid, appw, ssid, pw;
uint8_t iR, iG, iB;
uint8_t id;
int batteryThreshold, voltsPerCount;
int cutoffThreshold = 1;

WiFiUDP udp;
byte packetBuffer[512];

ESP8266WebServer server(80); // for configuration purposes
String escapeParameter(String param) {
  param.replace("+"," ");
  param.replace("%21","!");
  param.replace("%23","#");
  param.replace("%24","$");
  param.replace("%26","&");
  param.replace("%27","'");
  param.replace("%28","(");
  param.replace("%29",")");
  param.replace("%2A","*");
  param.replace("%2B","+");
  param.replace("%2C",",");
  param.replace("%2F","/");
  param.replace("%3A",":");
  param.replace("%3B",";");
  param.replace("%3D","=");
  param.replace("%3F","?");
  param.replace("%40","@");
  param.replace("%5B","[");
  param.replace("%5D","]");
  
  return param;
}
void handleRoot() {
  // header
  String str = "<html><head><title>Wifitally " + String(id+1) + " Config</title></head><body><form action=\"/update\" method=\"GET\" >";
  //elements
  str += "<h2>Wifi Settings</h2>";
  str += "ID: <select name=\"id\" />";
  for(int i=0; i<8; i++) {
    String b = "";
    if(i==id) {
      b = " selected";
    }
    str+= "<option value=\""+ String(i) + "\"" + b + ">" + String(i+1) + "</option>";
  }
  str += "</select><br/>";
  str += "Standalone AP SSID: <input type='text' name='apssid' value='" + String(apssid) + "' /><br/>";
  str += "Standalone AP Password: <input type='text' name='appw' value='" + String(appw) + "' /><br/>";
  String checked = "";
  if(standalone) {
    checked = "checked='checked'";
  }
  str += "Standalone: <input type='checkbox' name='standalone' value='y' " + checked + "/><br/>";
  str += "WLAN SSID: <input type='text' name='ssid' value='" + String(ssid) + "' /><br/>";
  str += "WLAN Password: <input type='text' name='pw' value='" + String(pw) + "' /><br/>";
  str += "<h2>Brightness Settings</h2>";
  str += "Intensity R: <input type='text' name='r' value='" + String(iR) + "' /><br/>";
  str += "Intensity G: <input type='text' name='g' value='" + String(iG) + "' /><br/>";
  str += "Intensity B: <input type='text' name='b' value='" + String(iB) + "' /><br/>";
  str += "<h2>Battery Warning Settings</h2>";
  str += "Current Voltage: <input type='number' name='curVoltage' min='0' max='12000' step='1' value='" + String(voltsPerCount*analogRead(A0)) + "'/>mV Measure the current battery voltage and enter it here<br/>";
  str += "Low Battery Voltage: <input type='number' name='lowVoltage' value='" + String(voltsPerCount*batteryThreshold) +"' min='0' max='12000' step='1'/>mV Enter the Voltage at which your battery is concidered empty. Default is 3300mV for LiPo <br/>";
  str += "Powerdown Voltage: <input type='number' name='cutoffVoltage' value='" + String(voltsPerCount*cutoffThreshold) +"' min='0' max='12000' step='1'/>mV Enter the Voltage at which the Tally shuts down, to prevent damage to the battery. Default is 3100mV for LiPo <br/>";
  str += "<input type=\"submit\" value=\"Speichern\">";
  str += "</form>";
  str += "<a href='/setThreshold'>Use current batterystate as low battery Warning</a>";
  str += "<p style='color:red;font-weight: bold;'>Changing Standalone mode or any of the ssid/password parameters requires restarting the tally. This will be done automatically after saving the data.</p>";
  str += "<h2>Informations</h2>";
  str += "<dl>";
  str += "<dt>RSSI:</dt><dd>" + String(WiFi.RSSI()) + "</dd>";
  str += "</dl>";
  // footer
  str += "</body></html>";
  server.send(200, "text/html", str);
}
void handleThreshold() {
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "Redirected to: /");
  batteryThreshold = analogRead(A0);
}
void handleUpdate() {
  bool restart = false;
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "Redirected to: /");
  setId(server.arg("id").toInt());
  String pApssid = escapeParameter(server.arg("apssid"));
  if(!apssid.equals(pApssid)) {
    apssid = pApssid;
    restart = true; 
  }
  String pAppw = escapeParameter(server.arg("appw"));
  if(!appw.equals(pAppw)) {
    appw = pAppw;
    restart = true; 
  }
  String pSsid = escapeParameter(server.arg("ssid"));
  if(!ssid.equals(pSsid)) {
    ssid = pSsid;
    restart = true; 
  }
  String pPw = escapeParameter(server.arg("pw"));
  if(!pw.equals(pPw)) {
    pw = pPw;
    restart = true; 
  }
  bool pStandalone = false;
  if(server.arg("standalone") == "y") {
    pStandalone = true; 
  }
  if(pStandalone != standalone) {
    standalone = pStandalone;
    restart = true;
  }
  if(server.arg("curVoltage") != "") {
    int curLevel = analogRead(A0);
    int curVoltage = server.arg("curVoltage").toInt();
    int lowVoltage = server.arg("lowVoltage").toInt();
    int cutoffVoltage = server.arg("cutoffVoltage").toInt();
    voltsPerCount = curVoltage/curLevel;
    batteryThreshold = lowVoltage/voltsPerCount; 
    cutoffThreshold = cutoffVoltage/voltsPerCount;
  }
  iR = server.arg("r").toInt();
  iG = server.arg("g").toInt();
  iB = server.arg("b").toInt();
  updateEeprom();
  EEPROM.commit();
  if(restart) {
    ESP.restart();
  }
}

// Powerdown esp if the voltage is below cutoff voltage;
void powerDown() {
  // inform the camera operator about shutdown
  for(int i=0; i<10;i++) {
   led(1,1,1);
   delay(100);
   led(0,0,0);
   delay(100); 
  }
  // inform the atem operator about shutdown
  char packet[2];
  packet[0] = 's';
  packet[1] = id + '0';
  udp.beginPacket(multicastAddress, 3000);
  udp.write(packet, 8);
  udp.endPacket();  
  // power down wifi
  WiFi.mode(WIFI_OFF);
  // powerdown led
  led(0,0,0);
  // deepSleep for maxtime, this should be enough time for someone to notice...
  ESP.deepSleep(2147483647);
}

void updateEeprom() {
  uint8_t low, high;
  low = batteryThreshold;
  high = batteryThreshold>>8;
  eeprom(300, low);
  eeprom(301, high);
  low = voltsPerCount;
  high = voltsPerCount>>8;
  eeprom(302, low);
  eeprom(303, high);
  low = cutoffThreshold;
  high = cutoffThreshold>>8;
  eeprom(304, low);
  eeprom(305, high);
  eeprom(196, id); // ID
  eeprom(197, iR); //Intensity Red
  eeprom(198, iG); //Intensity Green
  eeprom(199, iB); //Intensity Blue
  eeprom(200, standalone);

  writeString(apssid, 98); // AP SSID
  writeString(appw, 131); // AP PW
  writeString(ssid, 201); // WLAN SSID
  writeString(pw, 240); // WLAN PW
}

void initTally() {
  Serial.println("initializing");
  id = 0;
  iR = 255;
  iG = 255;
  iB = 255;
  batteryThreshold = 165;
  voltsPerCount = 20;
  cutoffThreshold = 155;
  apssid = defaultSSID;
  appw = defaultPassword;
  standalone = true;
  ssid = "";
  pw = "";
  updateEeprom();
  Serial.println("initialized");
  EEPROM.commit();
  led(1,1,1);
  delay(500);
  led(0,0,0);
  delay(500);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  // Connect to WiFi network
  EEPROM.begin(4096); 
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(taster, INPUT); 
  if(digitalRead(taster)) {
    unsigned long now = millis();
    while(digitalRead(taster)) {
      unsigned long diff = millis() - now;
      Serial.print(",");
      if(diff > 5000) { // if button pressed 5 sek
        Serial.println("test");
        initTally();
        break;
      }
    }
  }

 

  id = EEPROM.read(196);
  apssid = readString(98);
  appw = readString(131);
  ssid = readString(201);
  pw = readString(240);
  iR = EEPROM.read(197);
  iG = EEPROM.read(198);
  iB = EEPROM.read(199);
  batteryThreshold = (EEPROM.read(301)<<8) + EEPROM.read(300);
  voltsPerCount = (EEPROM.read(303)<<8) + EEPROM.read(302);
  cutoffThreshold = (EEPROM.read(305)<<8) + EEPROM.read(304);
  standalone = EEPROM.read(200);
  Serial.println(id);
  
  char as[32], ap[64], a[32], p[64];;
  if(standalone) {
    apssid.toCharArray(as, 32);
    appw.toCharArray(ap, 64);
    Serial.println("Wifi AP SSID: " +apssid + " PW: " + appw);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(as, ap);
    udp.beginMulticast(WiFi.softAPIP(), multicastAddress, 3000);
  } else {
    WiFi.mode(WIFI_STA);
    ssid.toCharArray(a, 32);
    pw.toCharArray(p, 64);
    WiFi.begin(a, p);
    udp.beginMulticast(WiFi.localIP(), multicastAddress, 3000);
  }
  showId();
  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.on("/setThreshold", handleThreshold);
  server.begin();
}

void checkConnected() {
  if(!standalone && WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting WiFi to " + ssid + " with password: " + pw); 
    while(WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      led(0,1,1);
      delay(200);
      led(0,0,0);
      delay(200);
    } 
    Serial.println("connected");
  }
  if(!printed) {
    
    Serial.println(WiFi.localIP());
    printed = true;
  }
    
  
}
void setId(uint8_t i) {
    id = i;
    eeprom(196, id);
    showId();
}
void showId() {
  for(int i = 0; i<=id; i++) {
    b(1);
    delay(150);
    b(0);
    delay(100);
  }
}
void loop() {
  checkConnected();
  server.handleClient();
  if(pressed>150 && !digitalRead(taster)) {
    setId((id+1)%8);
  }
//  if(printes>1000) {
//    Serial.println(voltsPerCount*analogRead(A0));
//    printes = 0;
//  }
//  printes++;
  if(!digitalRead(taster)) {
   pressed = 0; 
  }
  if(digitalRead(taster)) {
    pressed++;
  }
  int noBytes = udp.parsePacket();
  if(noBytes) {
    readData(noBytes); 
  }
  if(changes) {
    EEPROM.commit();
    changes = false;
  }
  if(analogRead(A0)<batteryThreshold) {
    sendLowBatteryWarning();
  }
  if(analogRead(A0)<cutoffThreshold) {
    powerDown();
  }
}

void sendLowBatteryWarning() {
  char packet[2];
  packet[0] = 'l';
  packet[1] = id + '0';
  udp.beginPacket(multicastAddress, 3000);
  udp.write(packet, 8);
  udp.endPacket();  
}

void eeprom(int address, uint8_t content) {
  EEPROM.write(address, content);
  changes = true;
}

void writeString(String string, int address) {
  int length = string.length();
  eeprom(address, length);
  for (int i = 0; i < length; i++){
    eeprom(address+1+i, (byte)string[i]);
  }
}
String readString(int address) {
  int length = EEPROM.read(address);
  String s = "";
  if(length>0) {
    char str[length+1];
    for(int i=0;i<length ;i++) {
      char buf = EEPROM.read(address+1+i);
      str[i] = buf;
    }
    str[length] = '\0';
    s = String(str);
  }
  return s;
}

void readData(int noBytes) {
  udp.read(packetBuffer,noBytes);
  if(char(packetBuffer[0]) =='d') {
    //identify();
    return;
  }
  if(noBytes != 8) {
   return; 
  }
  char mine = char(packetBuffer[id]);
  if(mine=='0') {
   off();
  } else if(mine=='1') {
   preview(); 
  } else if(mine=='2') {
   live(); 
  }
}

void preview() {
  led(0,1,0);
}

void live() {
  led(1,0,0);
}
void off() {
  led(0,0,0);
}

void led(bool red, bool green, bool blue) {
  r(red);
  g(green);
  b(blue);
}
void r(bool set) {
  if(set) {
    analogWrite(ledR, iR); 
  } else {
    analogWrite(ledR, 0); 
  }
}
void g(bool set) {
  if(set) {
    analogWrite(ledG, iG); 
  } else {
    analogWrite(ledG, 0); 
  }
}
void b(bool set) {
  if(set) {
    analogWrite(ledB, iB); 
  } else {
    analogWrite(ledB, 0); 
  }
}
