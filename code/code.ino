#include <WiFi.h>
#include "secret.h"
#include <WebServer.h>

// --- Configuration ---
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* pc_ip = IP_ADDRESS; 
const char* key = API_KEY;

WebServer server(80);

// HTML Template
const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP32 Control</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body { font-family: Arial; text-align: center; background: #f4f4f4; }
  button { padding: 12px; margin: 5px; width: 80px; cursor: pointer; }
  textarea { width: 80%; height: 60px; margin-top: 10px; }
  .stat-box { font-weight: bold; color: #333; }
</style>
</head>
<body>

<h3>ESP IP: %ESP_IP%</h3>

<h2>System</h2>
<button onclick="req('/lock')">Lock</button>
<button onclick="req('/shutdown')">Off</button>

<h2>Media</h2>
<button onclick="req('/media/play')">Play</button>
<button onclick="req('/media/next')">Next</button>
<button onclick="req('/media/prev')">Prev</button><br>
<button onclick="req('/media/volup')">Vol +</button>
<button onclick="req('/media/voldown')">Vol -</button>
<button onclick="req('/media/mute')">Mute</button>

<h2>Save Text</h2>
<textarea id="txt"></textarea><br>
<button onclick="saveText()" style="width:100px">Save</button>

<div class="stat-box">
  <p>CPU: <span id="cpu">0</span>% | RAM: <span id="ram">0</span>%</p>
</div>

<script>
const base = "http://%PC_IP%:5000";

function req(path){
  fetch(`${base}${path}?key=%KEY%`).catch(err => console.error(err));
}

function saveText(){
  let text = document.getElementById("txt").value;
  fetch(`${base}/save?key=%KEY%`, {
    method: "POST",
    headers: {"Content-Type": "application/json"},
    body: JSON.stringify({text: text})
  });
}

setInterval(async ()=>{
  try {
    let res = await fetch(`${base}/stats?key=%KEY%`);
    let data = await res.json();
    document.getElementById("cpu").innerText = data.cpu;
    document.getElementById("ram").innerText = data.ram;
  } catch(e) {}
}, 2000);
</script>

</body>
</html>
)rawliteral";

void handleRoot() {
  String page = String(html);
  String espIP = WiFi.localIP().toString();
  
  // Perform replacements
  page.replace("%PC_IP%", pc_ip);
  page.replace("%KEY%", key);
  page.replace("%ESP_IP%", espIP);
  
  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("ESP IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}