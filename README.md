# 🚀 AsyncWiFiManagerSimple

<p align="center">
  <img src="docs/logo.svg" width="180" alt="AsyncWiFiManagerSimple Logo">
</p>

<p align="center">
  <b>Lightweight Async WiFi Manager for ESP32</b><br>
  Captive Portal • Dynamic WiFi Scanning • No SPIFFS • PROGMEM HTML
</p>

<p align="center">
  <img src="https://img.shields.io/badge/version-1.0.0-blue.svg">
  <img src="https://img.shields.io/badge/license-MIT-green.svg">
  <img src="https://img.shields.io/badge/platform-ESP32-orange.svg">
  <img src="https://github.com/Popa/AsyncWiFiManagerSimple/actions/workflows/build.yml/badge.svg">
</p>

---

## 📘 Overview

**AsyncWiFiManagerSimple** is a lightweight, fully asynchronous WiFi configuration manager for **ESP32**, designed for reliability, simplicity, and minimal flash usage.

It provides a modern captive portal, dynamic WiFi scanning, multi-network storage, and automatic reconnection — all **without SPIFFS or external files**.

The entire configuration page is stored in **PROGMEM**, making the library ideal for compact IoT devices, long-running systems, and production-grade embedded applications.

---

## 📦 Installation

### ✅ Arduino Library Manager (recommended)

1. Open **Arduino IDE**
2. Go to **Sketch → Include Library → Manage Libraries…**
3. Search for **AsyncWiFiManagerSimple**
4. Click **Install**

> ⚠️ Available after the library is accepted into the Arduino Library Registry.

---

### 📁 Install from GitHub (ZIP)

1. Download the latest release from GitHub  
   **Code → Download ZIP**
2. Open **Arduino IDE**
3. Go to **Sketch → Include Library → Add .ZIP Library**
4. Select the downloaded ZIP file

---

### 🔧 Dependencies

This library requires the following dependencies:

- **ESPAsyncWebServer**
- **AsyncTCP** (ESP32)
- **DNSServer** (included with ESP32 core)

Make sure they are installed via **Library Manager**.

---

## 🧩 Architecture

The library is built on four core components:

### 1️⃣ ESPAsyncWebServer
- Fully asynchronous HTTP server  
- Serves HTML directly from PROGMEM  
- Non-blocking and highly performant  

### 2️⃣ DNSServer
- Redirects all DNS queries to the ESP32 AP  
- Creates a true captive portal experience  

### 3️⃣ Preferences (NVS)
- Stores up to **4 WiFi networks**
- Persistent across reboots  
- No filesystem required  

### 4️⃣ WiFi + Dynamic Scanning
- Real-time WiFi scanning  
- Sorted by RSSI  
- Sequential connection attempts to saved networks  

---

## 🔄 How It Works

### 1️⃣ Startup Sequence

On boot:
- Serial and NVS are initialized  
- Saved networks are loaded  
- ESP32 attempts to connect to each network  
- If all attempts fail → **AP Mode starts automatically**
 
## Example Usage ##

**Default AP credentials**
```cpp
#include <AsyncWiFiManagerSimple.h>

AsyncWiFiManagerSimple wifiManager;

void setup() {
  wifiManager.Setup();  

void loop() {
  wifiManager.loop();
}
```
**Custom AP SSID and password**
```cpp
wifiManager.Setup("MyAP", "MyPassword");

