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

🧩 1. Arhitectura generală
Biblioteca este construită în jurul a patru componente principale:

1.1. ESPAsyncWebServer
Gestionează serverul HTTP asincron.

Permite servirea paginii HTML direct din PROGMEM.

Asigură răspunsuri rapide și non‑blocante.

1.2. DNSServer
Interceptează toate cererile DNS.

Redirecționează orice domeniu către IP‑ul AP‑ului.

Creează efectul de captive portal.

1.3. Preferences (NVS)
Stochează rețelele WiFi salvate.

Persistă datele între restarturi.

Nu necesită sistem de fișiere.

1.4. WiFi + scanare dinamică
Scanare în timp real a rețelelor disponibile.

Sortare după RSSI în interfața web.

Conectare secvențială la rețelele salvate.

🔄 2. Fluxul de funcționare
2.1. Inițializare (Setup)
La pornire:

Se inițializează Serial și Preferences.

Se încarcă rețelele salvate din NVS.

Se încearcă conectarea la fiecare rețea.

Dacă toate încercările eșuează → se pornește modul AP.

Biblioteca suportă două moduri de inițializare:

Mod implicit
cpp
wifiManager.Setup();
Mod cu SSID și parolă AP personalizate
cpp
wifiManager.Setup("MyAP", "MyPassword");
2.2. Modul AP (Config Mode)
Dacă ESP32 nu se poate conecta la nicio rețea salvată:

se activează modul Access Point

se pornește serverul web

se pornește DNS serverul pentru captive portal

se servește pagina HTML din PROGMEM

Avantaje:
nu necesită SPIFFS

nu necesită fișiere externe

nu crește consumul de flash

2.3. Portalul web
Portalul include:

scanare WiFi în timp real (/scan)

sortare după puterea semnalului

selectoare pentru două rețele

câmpuri pentru parole

buton pentru ștergerea tuturor rețelelor salvate

Pagina este stocată în PROGMEM:

nu ocupă RAM

nu necesită sistem de fișiere

este servită instant

2.4. Salvarea rețelelor
La POST /add:

se șterg rețelele vechi

se salvează până la 4 rețele

se repornește automat dispozitivul

La POST /clear:

se șterg toate rețelele

se repornește dispozitivul

2.5. Reconectare automată
În modul normal:

dacă WiFi pică → se încearcă reconectarea la fiecare secundă

dacă numărul de încercări depășește RECONNECT_ATTEMPTS → restart automat

dacă reconectarea reușește → se afișează IP‑ul local

Acest mecanism asigură:

stabilitate pe termen lung

recuperare automată după pierderea semnalului

funcționare autonomă fără intervenție umană

🧠 3. Design intern și decizii tehnice
3.1. De ce PROGMEM în loc de SPIFFS?
Elimină necesitatea montării SPIFFS.

Reduce riscul de corupere a sistemului de fișiere.

Reduce consumul de flash.

Simplifică distribuirea bibliotecii (un singur fișier .h/.cpp).

3.2. De ce ESPAsyncWebServer?
non‑blocant

suport pentru multiple conexiuni simultane

ideal pentru portaluri captive

performanță superioară față de WebServer clasic

3.3. De ce Preferences (NVS)?
stocare sigură și persistentă

fără overhead de filesystem

acces rapid la date

📡 4. Interfața web
Portalul este:

responsive

minimalist

modern

compatibil cu toate browserele

generat 100% dinamic

Scanarea WiFi este realizată prin:

js
fetch('/scan')
Rezultatul este sortat după RSSI și populat în <select>.

🛡️ 5. Mecanisme de siguranță
Biblioteca include:

✔ watchdog de reconectare
✔ restart automat după prea multe încercări
✔ fallback în AP Mode
✔ timeout configurabil pentru portal
✔ protecție la salvarea rețelelor invalide
Aceste mecanisme fac biblioteca potrivită pentru:

sisteme IoT autonome

dispozitive industriale

echipamente care trebuie să ruleze luni/ani fără intervenție
----
## 📦 Instalare

### Arduino IDE
1. Copiază folderul `AsyncWiFiManagerSimple` în: Documents/Arduino/libraries/
2. Repornește Arduino IDE.

### PlatformIO
Adaugă în `platformio.ini`:


🧪 6. Exemplu de utilizare complet

cpp
#include <AsyncWiFiManagerSimple.h>

AsyncWiFiManagerSimple wifiManager;

void setup() {
  wifiManager.Setup("MyDevice_Config", "MyPassword123");
}

void loop() {
  wifiManager.loop();
}
🚀 7. Avantaje tehnice
zero fișiere externe

zero SPIFFS

zero blocări

HTML în PROGMEM

asincron

fallback automat

debug complet

footprint minim în flash
}

