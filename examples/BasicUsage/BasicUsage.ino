#include <AsyncWiFiManagerSimple.h>

AsyncWiFiManagerSimple wifiManager;

void setup() {
  wifiManager.Setup();  //wifiManager.Setup("ap_ssid" , "ap_pass); //
}

void loop() {
  wifiManager.loop();
}
