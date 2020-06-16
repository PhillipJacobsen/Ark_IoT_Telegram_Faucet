/********************************************************************************
  This file contains various fuctions
*****


/********************************************************************************
  send MQTT Packet periodically every UpdateInterval_MQTT_Publish
********************************************************************************/
void send_MQTTpacket() {

  if (millis() - previousUpdateTime_MQTT_Publish > UpdateInterval_MQTT_Publish)  {
    previousUpdateTime_MQTT_Publish += UpdateInterval_MQTT_Publish;

    if (WiFiMQTTclient.isMqttConnected()) {

      String  buf;
      long rssi = WiFi.RSSI();

      buf = String(rssi);

      Serial.println();
      Serial.print("sent MQTT topic IOT/set: ");
      Serial.println(buf);

      const char* MQTT_Base_Topic = "IOT/status";

      WiFiMQTTclient.publish(MQTT_Base_Topic, buf);

    }
  }
}



/********************************************************************************
  Copy data from nonvolatile memory into RAM.
  Note. ESP32 has FLASH memory(not EEPROM) however the standard high level Arduino EEPROM arduino functions work.
********************************************************************************/
// Load data from EEPROM
// You need to call EEPROM.begin(size) before you start reading or writing, size being the number of bytes you want to use.
// Size can be anywhere between 4 and 4096 bytes.

//void loadEEPROM() {
//  EEPROM.begin(512);
//  EEPROM.get(0, lastRXpage_eeprom);
//  // EEPROM.get(0 + sizeof(ssid), password);
//  // EEPROM.get(0 + sizeof(ssid) + sizeof(password), coinname);
//  char ok[2 + 1];
//  //EEPROM.get(0+sizeof(ssid)+sizeof(password), ok);
//  //EEPROM.get(0 + sizeof(ssid) + sizeof(password) + sizeof(coinname), ok);
//  EEPROM.get(0 + sizeof(lastRXpage_eeprom), ok);
//  EEPROM.end();
//  if (String(ok) != String("OK")) {
//    lastRXpage_eeprom = 0;
//  }
//  Serial.println("Recovered credentials from FLASH");
//  Serial.println(ok);
//  Serial.println(lastRXpage_eeprom);
//  // Serial.println(strlen(ssid) > 0 ? ssid : "<no ssid>");
//  // Serial.println(strlen(password) > 0 ? "********" : "<no password>");
//  //Serial.println(strlen(coinname) > 0 ? coinname : "<no coinname>");
//}


/********************************************************************************
  Store data in nonvolatile memory.
  Note. ESP32 has FLASH memory(not EEPROM) however the standard high level Arduino EEPROM arduino functions work.
  EEPROM.write does not write to flash immediately, instead you must call EEPROM.commit() whenever you wish to save changes to flash. EEPROM.end() will also commit, and will release the RAM copy of EEPROM contents.

********************************************************************************/
//void saveEEPROM() {
//  EEPROM.begin(512);
//  EEPROM.put(0, lastRXpage_eeprom);
//  // EEPROM.put(0 + sizeof(ssid), password);
//  // EEPROM.put(0 + sizeof(ssid) + sizeof(password), coinname);
//  char ok[2 + 1] = "OK";
//  //EEPROM.put(0+sizeof(ssid)+sizeof(password), ok);
//  //EEPROM.put(0 + sizeof(ssid) + sizeof(password) + sizeof(coinname), ok);
//  EEPROM.put(0 + sizeof(lastRXpage_eeprom), ok);
//  EEPROM.commit();
//  EEPROM.end();
//  Serial.println("Saved credentials to FLASH");
//}
