# AsyncWiFiManagerSimple

<p align="center">
  <img src="docs/logo.svg" width="180" alt="AsyncWiFiManagerSimple Logo">
</p>

<p align="center">
  <b>Lightweight Async WiFi Manager for ESP32</b><br>
  Captive portal • Dynamic WiFi scanning • No SPIFFS • PROGMEM HTML
</p>

<p align="center">
  <img src="https://img.shields.io/badge/version-1.0.0-blue.svg">
  <img src="https://img.shields.io/badge/license-MIT-green.svg">
  <img src="https://img.shields.io/badge/platform-ESP32-orange.svg">
  <img src="https://github.com/Popa/AsyncWiFiManagerSimple/actions/workflows/build.yml/badge.svg">
</p>

---

## ✨ Caracteristici

- Captive portal automat dacă nu există conexiune WiFi
- Scanare WiFi dinamică cu sortare după RSSI
- Salvarea a până la 4 rețele WiFi în NVS (Preferences)
- HTML modern integrat în PROGMEM (zero SPIFFS)
- Reîncercare automată + watchdog de reconectare
- Compatibil cu **ESPAsyncWebServer**
- Zero fișiere externe → ideal pentru proiecte compacte

---

## 📦 Instalare

### Arduino IDE
1. Copiază folderul `AsyncWiFiManagerSimple` în: Documents/Arduino/libraries/
2. Repornește Arduino IDE.

### PlatformIO
Adaugă în `platformio.ini`:

```ini
lib_deps =
 AsyncWiFiManagerSimple

Exemplu de utilizare:
#include <AsyncWiFiManagerSimple.h>

AsyncWiFiManagerSimple wifiManager;

void setup() {
  wifiManager.Setup();
}

void loop() {
  wifiManager.loop();
}
