#include <ESPAsyncWebServer.h>

const char onboarding_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><head>
    <title>WiFiTally</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="styles.css">
    <style>
      .fade { opacity: 1; visibility: visible; transition-duration: 200ms; transition-property: opacity, max-height, visibility; max-height: 800px;}
      .hidden { opacity: 0; visibility: hidden; transition-duration: 200ms; transition-property: opacity, max-height, visibility; max-height: 0px; cursor: none;}
    </style>
    </head><body>
  <div class="container">
    <div class="text-center mt-2">
      <h1>WiFiTally setup</h1>
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
            <dt>Version</dt>
            <dd>%version%</dd>
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
                <input class="form-input" type="text" value="" id="ssid" placeholder="SSID" name="ssid">
              </div>
            </div>
            <div class="form-group">
              <div class="col-4 col-sm-12">
                <label for="password" class="form-label">Password</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="password" value="" id="password" placeholder="**********" name="password">
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
                  <input type="checkbox" name="useDHCP" id="useDHCP" value="true" checked onchange="toggleVisibility(this, 'network-group', false)">
                  <i class="form-icon"></i>
                  Use DHCP
                </label>
              </div>
            </div>
            <div class="form-group network-group fade">
              <div class="col-4 col-sm-12">
                <label for="ip" class="form-label">IP</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="text" value="0.0.0.0" id="ip" placeholder="0.0.0.0" pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" name="ip">
              </div>
            </div>
            <div class="form-group network-group fade">
              <div class="col-4 col-sm-12">
                <label for="subnet" class="form-label">Subnet Mask</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="text" value="255.255.255.0" id="subnet" placeholder="255.255.255.0" pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" name="subnet">
              </div>
            </div>
            <div class="form-group network-group fade">
              <div class="col-4 col-sm-12">
                <label for="gateway" class="form-label">Gateway</label>
              </div>
              <div class="col-8 col-sm-12">
                <input class="form-input" type="text" value="0.0.0.0" id="gateway" placeholder="0.0.0.0" pattern="((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$" name="gateway">
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
    toggleVisibility(document.getElementById('enableBackLED'), 'led-group', true);
    toggleVisibility(document.getElementById('useDHCP'), 'network-group', false);
  </script>
  
  </body></html>)rawliteral";