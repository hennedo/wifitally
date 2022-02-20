#include <ESPAsyncWebServer.h>

const char onboarding_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>WifiTally Onboarding</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="/styles.css">
  </head><body>
<h1>WifiTally Onboarding</h1>
<br />
<div class="container">
  <div class="row">
    <div class="col-sm">
    </div>
    <div class="col-sm-12 col-md-8 col-lg-6" style="height: calc(100vh - 10.25rem); display: flex; align-items: center; flex: 0 1 auto;">
      <form action="/configure" method="POST">
        <fieldset>
          <legend>Wifi Config</legend>
            <div class="input-group fluid">
              <label for="ssid" style="width: 80px;">SSID</label>
              <input type="text" value="" id="ssid" placeholder="SSID" name="ssid">
            </div>
            <div class="input-group fluid">
              <label for="pwd" style="width: 80px;">Password</label>
              <input type="password" value="" id="pwd" placeholder="password" name="password">
            </div>
            <div class="input-group">
                <input type="checkbox" id="useDHCP" name="useDHCP" value="true">
                <label for="useDHCP">Use DHCP</label>
            </div>
            <div class="input-group fluid">
              <label for="ip" style="width: 80px;">IP</label>
              <input type="text" value="0.0.0.0" id="ip" placeholder="0.0.0.0" pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" name="ip">
            </div>
            <div class="input-group fluid">
              <label for="subnet" style="width: 80px;">Subnet Mask</label>
              <input type="text" value="255.255.255.0" id="subnet" placeholder="255.255.255.0" pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" name="subnet">
            </div>
            <div class="input-group fluid">
              <label for="gateway" style="width: 80px;">Gateway</label>
              <input type="text" value="0.0.0.0" id="gateway" placeholder="0.0.0.0" pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" name="gateway">
            </div>
            <div class="input-group fluid">
              <button class="primary">Save</button>
            </div>
        </fieldset>
      </form>
    </div>
    <div class="col-sm">
    </div>
  </div>
</div>
<footer>
    <p>Copyright &copy;2022 @hennedo</p>
</footer>

</body></html>)rawliteral";