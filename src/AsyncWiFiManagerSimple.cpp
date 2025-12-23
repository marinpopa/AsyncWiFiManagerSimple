#include "AsyncWiFiManagerSimple.h"

// ---------------------------------------------------------
// HTML PAGE IN PROGMEM
// ---------------------------------------------------------
const char WIFI_CONFIG_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Configurare WiFi</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; background: #f0f0f0; padding: 20px; }
    .container { background: white; padding: 20px; border-radius: 10px; max-width: 500px; margin: auto; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
    select, input { width: 100%; padding: 12px; margin: 8px 0; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }
    button { background: #4CAF50; color: white; padding: 14px 20px; border: none; border-radius: 4px; cursor: pointer; width: 100%; }
    button:hover { background: #45a049; }
    .clear-btn { background: #f44336; margin-top: 20px; }
  </style>
</head>
<body>
  <div class="container">
    <h2>Configurare WiFi</h2>
    <form action="/add" method="post">
      <select name="ssid1"></select>
      <input type="text" name="password1" placeholder="Parola WiFi 1">
      <select name="ssid2"></select>
      <input type="text" name="password2" placeholder="Parola WiFi 2">
      <button type="submit">Aplica & Restart</button>
    </form>
    <form action="/clear" method="post">
      <button type="submit" class="clear-btn">Sterge toate retelele salvate</button>
    </form>
  </div>

  <script>
  function sortNetworks(networks) {
    return networks.sort((a, b) => b.rssi - a.rssi);
  }

  function populateNetworks() {
    fetch('/scan')
      .then(response => response.json())
      .then(networks => {
        networks = sortNetworks(networks);
        const selects = document.querySelectorAll('select');
        selects.forEach(select => {
          select.innerHTML = '<option value="">Selecteaza SSID</option>';
          networks.forEach(network => {
            const option = document.createElement('option');
            option.value = network.ssid;
            option.textContent = `${network.ssid} (${network.rssi} dBm)`;
            select.appendChild(option);
          });
        });
      })
      .catch(error => console.error('Scanare e?uata:', error));
  }

  document.addEventListener('DOMContentLoaded', populateNetworks);
  </script>
</body>
</html>
)rawliteral";

// ---------------------------------------------------------
// SETUP METHODS
// ---------------------------------------------------------

void AsyncWiFiManagerSimple::Setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("=== AsyncWiFiManagerSimple START ===");

    Serial.printf("[AP] SSID: %s\n", apSSID);
    Serial.printf("[AP] PASS: %s\n", apPASS);

    preferences.begin("wifi-config", false);
    loadSavedNetworks();

    Serial.printf("[WiFi] Retele salvate: %d\n", networkCount);
    for (int i = 0; i < networkCount; i++) {
        Serial.printf("  %d) %s\n", i + 1, savedNetworks[i].ssid.c_str());
    }

    if (!connectToSavedNetwork()) {
        Serial.println("[WiFi] Nu s-a putut conecta la nicio retea salvata.");
        startConfigMode();
    }
}

void AsyncWiFiManagerSimple::Setup(const char* ap_ssid, const char* ap_pass) {
    apSSID = ap_ssid;
    apPASS = ap_pass;

    Serial.println("[AP] Setari AP primite dinamic:");
    Serial.printf("     SSID: %s\n", apSSID);
    Serial.printf("     PASS: %s\n", apPASS);

    Setup();
}

// ---------------------------------------------------------
// LOOP
// ---------------------------------------------------------

void AsyncWiFiManagerSimple::loop() {
    if (inConfigMode) {
        dnsServer.processNextRequest();
        if (millis() - configStartTime > CONFIG_TIMEOUT * 1000) {
            Serial.println("[AP] Timeout configurare. Restart...");
            ESP.restart();
        }
    } else {
        checkWiFiConnection();
    }
}

// ---------------------------------------------------------
// LOAD SAVED NETWORKS
// ---------------------------------------------------------

void AsyncWiFiManagerSimple::loadSavedNetworks() {
    networkCount = preferences.getInt("count", 0);

    for (int i = 0; i < networkCount; i++) {
        char key[10];
        snprintf(key, sizeof(key), "ssid%d", i);
        savedNetworks[i].ssid = preferences.getString(key, "");

        snprintf(key, sizeof(key), "pass%d", i);
        savedNetworks[i].pass = preferences.getString(key, "");
    }
}

// ---------------------------------------------------------
// CONNECT TO SAVED NETWORKS
// ---------------------------------------------------------

// ---------------------------------------------------------
// CONNECT TO SAVED NETWORKS
// ---------------------------------------------------------

bool AsyncWiFiManagerSimple::connectToSavedNetwork() {
    for (int i = 0; i < networkCount; i++) {
        Serial.printf("[WiFi] Incerc conectarea la: %s\n", savedNetworks[i].ssid.c_str());

        WiFi.begin(savedNetworks[i].ssid.c_str(), savedNetworks[i].pass.c_str());
        wl_status_t result = (wl_status_t)WiFi.waitForConnectResult(10000);

        if (result == WL_CONNECTED) {
            Serial.printf("[WiFi] Conectat la %s\n", savedNetworks[i].ssid.c_str());
            Serial.printf("[WiFi] IP local: %s\n", WiFi.localIP().toString().c_str());
            return true;
        }

        Serial.printf("[WiFi] Eroare conectare (%d)\n", result);
    }

    return false;
}

// ---------------------------------------------------------
// START CONFIG MODE (AP + CAPTIVE PORTAL)
// ---------------------------------------------------------

void AsyncWiFiManagerSimple::startConfigMode() {
    Serial.println("[AP] Pornesc modul Access Point...");
    Serial.printf("[AP] SSID: %s\n", apSSID);
    Serial.printf("[AP] PASS: %s\n", apPASS);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPASS);

    Serial.printf("[AP] IP AP: %s\n", WiFi.softAPIP().toString().c_str());

    dnsServer.start(53, "*", WiFi.softAPIP());

    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", WIFI_CONFIG_PAGE);
    });

    server.on("/scan", HTTP_GET, [&](AsyncWebServerRequest *request) {
        int n = WiFi.scanNetworks();
        String json = "[";

        for (int i = 0; i < n; i++) {
            if (i) json += ",";
            json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
        }

        json += "]";
        request->send(200, "application/json", json);
    });

    server.on("/add", HTTP_POST, [&](AsyncWebServerRequest *request) {
        handleSave(request);
    });

    server.on("/clear", HTTP_POST, [&](AsyncWebServerRequest *request) {
        handleDelete(request);
    });

    server.begin();
    inConfigMode = true;
    configStartTime = millis();
}

// ---------------------------------------------------------
// SAVE NETWORKS
// ---------------------------------------------------------

void AsyncWiFiManagerSimple::handleSave(AsyncWebServerRequest *request) {
    preferences.clear();
    int saved = 0;

    for (int i = 1; i <= 2; i++) {
        String ssidParam = "ssid" + String(i);
        String passParam = "password" + String(i);

        if (request->hasParam(ssidParam, true) && request->hasParam(passParam, true)) {
            String ssid = request->getParam(ssidParam, true)->value();
            String pass = request->getParam(passParam, true)->value();

            if (ssid.length() > 0 && saved < MAX_NETWORKS) {
                char key[10];
                snprintf(key, sizeof(key), "ssid%d", saved);
                preferences.putString(key, ssid);

                snprintf(key, sizeof(key), "pass%d", saved);
                preferences.putString(key, pass);

                saved++;
            }
        }
    }

    preferences.putInt("count", saved);
    request->send(200, "text/plain", "Setari salvate! Restart...");
    delay(500);
    ESP.restart();
}

// ---------------------------------------------------------
// DELETE NETWORKS
// ---------------------------------------------------------

void AsyncWiFiManagerSimple::handleDelete(AsyncWebServerRequest *request) {
    preferences.clear();
    request->send(200, "text/plain", "Setari sterse! Restart...");
    delay(500);
    ESP.restart();
}

// ---------------------------------------------------------
// CHECK CONNECTION + RECONNECT
// ---------------------------------------------------------

void AsyncWiFiManagerSimple::checkWiFiConnection() {
    static unsigned long lastReconnectAttempt = 0;
    static bool wasConnected = false;

    bool isConnected = (WiFi.status() == WL_CONNECTED);

    // --- ADAUGĂ AFIȘARE CÂND SE PIERDE CONEXIUNEA ---
    if (wasConnected && !isConnected) {
        Serial.println("[WiFi] ⚠️  Conexiune pierduta!");
        Serial.println(WiFi.localIP().toString());
        Serial.print("[WiFi] SSID pierdut: ");
       // Serial.println(WiFi.SSID());
    }

    if (isConnected && !wasConnected) {
        Serial.println("[WiFi] ✅ Conexiune restabilita.");
        Serial.printf("[WiFi] IP local: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("[WiFi] SSID: %s\n", WiFi.SSID().c_str());
        Serial.printf("[WiFi] RSSI: %d dBm\n", WiFi.RSSI());
        reconnectAttempts = 0;
    }

    if (!isConnected) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt >= 3000) {
            lastReconnectAttempt = now;
            reconnectAttempts++;

            Serial.printf("[WiFi] 🔄 Incercare reconectare #%d...\n", reconnectAttempts);

            if (reconnectAttempts >= RECONNECT_ATTEMPTS) {
                Serial.println("[WiFi] ⛔ Prea multe incercari. Restart...");
                ESP.restart();
            }

            if (!connectToSavedNetwork()) {
                Serial.println("[WiFi] ❌ Nicio retea salvata disponibila.");
            }
        }
    }

    wasConnected = isConnected;
}
