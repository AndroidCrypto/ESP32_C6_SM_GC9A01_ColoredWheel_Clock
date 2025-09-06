/*
  All Wi-Fi related stuff is here
*/

uint8_t StartWiFi() {
  Serial.println("\r\nConnecting to: " + String(WIFI_SSID));
  IPAddress dns(8, 8, 8, 8);  // Use Google DNS
  WiFi.disconnect();
  WiFi.mode(WIFI_STA); // switch off AP
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(500);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
  if (WiFi.status() == WL_CONNECTED) {
    wiFiRssi = WiFi.RSSI();  // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
    Serial.println("WiFi connected at: " + WiFi.localIP().toString());
  } else Serial.println("WiFi connection *** FAILED ***");
  return WiFi.status();
}

void StopWiFi() {
  Serial.println("WiFi disconnect");
  WiFi.disconnect();
  Serial.println("WiFi.mode OFF");
  WiFi.mode(WIFI_OFF);
}