#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><head>
    <title>WiFiTally</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="styles.css">
    <style>
      .fade { opacity: 1; visibility: visible; transition-duration: 200ms; transition-property: opacity, max-height, visibility; max-height: 800px;}
      .hidden { opacity: 0; visibility: hidden; transition-duration: 200ms; transition-property: opacity, max-height, visibility; max-height: 0px; cursor: none;}
    </style>
    </head><body>
  <div class="container">
    <div class="columns mt-2">
      <dic class="column col-2"></dic>
      <div class="column col-8">
        <h1>WiFiTally settings</h1>
      </div>
    </div>
    <div class="columns mt-2">
      <div class="column col-2"></div>
      <div class="card column col-8">
        <div class="card-header">
          <div class="card-title h5">Tally Information</div>
          <div class="card-subtitle text-gray">well. its information!</div>
        </div>
        <div class="card-body">
          <dl>
            <dt>Hostname</dt>
            <dd>%hostname%</dd>
            <dt>ID</dt>
            <dd>%id%</dd>
            <dt>Version</dt>
            <dd>%version%</dd>
            <dt>IP</dt>
            <dd>%current_ip%</dd>
            <dt>Battery</dt>
            <dd>
            %battery_voltage%V - %battery_percentage%&#37;
            <progress class="progress" value="%battery_percentage%" max="100"></progress>
            </dd>
          </dl>
        </div>
        
        <div class="card-footer">
        </div>
      </div>
    </div>
    <div class="columns mt-2">
      <div class="column col-2"></div>
      <div class="card column col-8">
        <form action="/configure" method="POST" class="form-horizontal">
          <div class="card-header">
            <div class="card-title h5">WiFi Configuration</div>
            <div class="card-subtitle text-gray">Requires a reboot.</div>
          </div>
          <div class="card-body">
            <div class="form-group">
              <div class="col-4 col-sm-12">
                <label for="ssid" class="form-label">SSID</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="text" value="%ssid%" id="ssid" placeholder="SSID" name="ssid">
              </div>
            </div>
            <div class="form-group">
              <div class="col-4 col-sm-12">
                <label for="password" class="form-label">Password</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="password" value="%password%" id="password" placeholder="**********" name="password">
                <label class="form-checkbox">
                  <input type="checkbox" onchange="togglePasswordVisibility(this, 'password')">
                  <i class="form-icon"></i>
                  Show Password
                </label>
              </div>
            </div>
            <div class="form-group">
              <div class="col-sm-12 col-8 col-ml-auto">
                <label class="form-switch">
                  <input type="checkbox" name="useDHCP" id="useDHCP" value="true" %useDHCP% onchange="toggleVisibility(this, 'network-group', false)">
                  <i class="form-icon"></i>
                  Use DHCP
                </label>
              </div>
            </div>
            <div class="form-group network-group fade %network%">
              <div class="col-4 col-sm-12">
                <label for="ip" class="form-label">IP</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="text" value="%ip%" id="ip" placeholder="0.0.0.0" pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" name="ip">
              </div>
            </div>
            <div class="form-group network-group fade %network%">
              <div class="col-4 col-sm-12">
                <label for="subnet" class="form-label">Subnet Mask</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="text" value="%subnet%" id="subnet" placeholder="255.255.255.0" pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" name="subnet">
              </div>
            </div>
            <div class="form-group network-group fade %network%">
              <div class="col-4 col-sm-12">
                <label for="gateway" class="form-label">Gateway</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="text" value="%gateway%" id="gateway" placeholder="0.0.0.0" pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" name="gateway">
              </div>
            </div>
          </div>
          
          <div class="card-footer">
            <button class="btn btn-primary">Save</button>
          </div>
        </form>
      </div>
    </div>
    <div class="columns mt-2">
      <div class="column col-2"></div>
      <div class="card column col-8">
        <form action="/settings" method="POST" class="form-horizontal">
          <div class="card-header">
            <div class="card-title h5">Tally Settings</div>
            <div class="card-subtitle text-gray">General settings for the tally</div>
          </div>
          <div class="card-body">
            <div class="form-group">
              <div class="col-sm-12 col-8 col-ml-auto">
                <label class="form-switch">
                  <input type="checkbox" name="showMiniOverview" value="true" %showMiniOverview%>
                  <i class="form-icon"></i>
                  Show mini overview
                </label>
              </div>
            </div>
            <div class="form-group">
              <div class="col-4 col-sm-12">
                <label for="id" class="form-label">ID</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="number" value="%id%" id="id" min="1" max="8" name="id">
              </div>
            </div>
            <div class="form-group">
              <div class="col-4 col-sm-12">
                <label for="brightness" class="form-label">Brightness</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="number" value="%brightness%" id="brightness" min="7" max="15" name="brightness">
                <small>(7 to 15)</small>
              </div>
            </div>
            <div class="form-group">
              <div class="col-sm-12 col-8 col-ml-auto">
                <label class="form-switch">
                  <input type="checkbox" name="enableBackLED" id="enableBackLED" value="true" onchange="toggleVisibility(this, 'led-group', true)" %enableBackLed%>
                  <i class="form-icon"></i>
                  Enable back LED
                </label>
              </div>
            </div>
            <div class="form-group led-group fade %led%">
              <div class="col-4 col-sm-12">
                <label for="ledBrightness" class="form-label">Back LED brightness</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="number" value="%ledBrightness%" id="ledBrightness" min="1" max="255" name="ledBrightness">
                <small>(1 to 255)</small>
              </div>
            </div>
          </div>
          <div class="card-footer">
            <button class="btn btn-primary">Save</button>
          </div>
        </form>
      </div>
    </div>
  </div>
  <div class="columns mt-2">
    <div class="column col-2"></div>
    <div class="card column col-8">
      <form action="/reset" method="POST" class="form-horizontal">
        <div class="card-header">
          <div class="card-title h5">Factory reset</div>
          <div class="card-subtitle text-gray">Danger: this will delete all settings!</div>
        </div>
        <div class="card-body">
       
        </div>
        <div class="card-footer">
          <button class="btn btn-error">Reset</button>
        </div>
      </form>
    </div>
  </div>
</div>
  <footer class="text-center mt-2">
      <p>made by <a href="https://github.com/hennedo/wifitally" target="_blank">@hennedo</a></p>
  </footer>

  <script type="text/javascript">
    function togglePasswordVisibility(src, target) {
      let elem = document.getElementById(target);
      if(src.checked) elem.type = 'text';
      else elem.type = 'password';
    }
    function toggleVisibility(src, group, compare) {
      let elems = document.getElementsByClassName(group);
      for(let i=0; i<elems.length; i++) {
        if(src.checked === compare) elems[i].classList.remove('hidden');
        else elems[i].classList.add('hidden');
      }
    }
  </script>
  
  </body></html>)rawliteral";