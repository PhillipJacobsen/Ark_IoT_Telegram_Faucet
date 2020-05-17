/********************************************************************************
  This file contains functions used to configure hardware peripherals and various libraries.
********************************************************************************/

/********************************************************************************
  Configure peripherals and system
********************************************************************************/
void setup()
{
  //--------------------------------------------
  // Enable Telegram Debug
  Bot.set_debug(DEBUG_LEVEL_UTLGBOT);

  //--------------------------------------------
  // Enable serial port and I/O
  Serial.begin(115200);           // Initialize Serial Connection for debug
  while ( !Serial && millis() < 20 );

  digitalWrite(LED_PIN, HIGH);    // Turn LED on
  pinMode(LED_PIN, OUTPUT);       // initialize on board LED control pin as an output.
  led_status = 0;

  // Optional Features of EspMQTTClient
  WiFiMQTTclient.enableDebuggingMessages(); // Enable debugging messages sent to serial output
//  WiFiMQTTclient.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overwritten with enableHTTPWebUpdater("user", "password").
  WiFiMQTTclient.enableLastWillMessage("IoT/lastwill", "Goodbye");  // You can activate the retain flag by setting the third parameter to true
  WiFiMQTTclient.enableMQTTConnect();
  WiFiMQTTclient.enableMACaddress_for_ClientName();


}


/********************************************************************************
  
********************************************************************************/
// Handler for receiving MQTT message
void set2Handler (const String & payload) {
  Serial.print("received MQTT topic IOT/set2: ");
  Serial.println(payload);
}




/********************************************************************************
  This function is called once WiFi and MQTT connections are complete
********************************************************************************/
void onConnectionEstablished() {

  if (!initialConnectionEstablished_Flag) {     //execute this the first time we have established a WiFi and MQTT connection after powerup
    initialConnectionEstablished_Flag = true;

    //--------------------------------------------
    //  sync local time to NTP server
    configTime(TIME_ZONE * 3600, DST, "pool.ntp.org", "time.nist.gov");

    //--------------------------------------------
    //  display IP address.
    Serial.println("\nIP address: ");
    Serial.println(WiFi.localIP());

    // Subscribe to "IOT/set" and display received message to Serial
    WiFiMQTTclient.subscribe("IOT/set", [](const String & payload) {
      Serial.print("received MQTT topic IOT/set: ");
      Serial.println(payload);
    });


    // Subscribe to "IOT/#" and display received message to Serial
    WiFiMQTTclient.subscribe("IOT/#", [](const String & topic, const String & payload) {
      Serial.print("received MQTT via wildcard: ");
      Serial.println(topic + ": " + payload);
    });

    // Subscribe to "IOT/set2" via alternate callback format
    WiFiMQTTclient.subscribe("IOT/set2", set2Handler);

    Serial.println("");
    Serial.println(WiFiMQTTclient.getMqttClientName());
    Serial.println(WiFiMQTTclient.getMqttServerIp());
    Serial.println(WiFiMQTTclient.getMqttServerPort());
    Serial.println(WiFiMQTTclient.getConnectionEstablishedCount());



    //--------------------------------------------
    //  Copy data stored in Flash into RAM
    //    loadEEPROM();                 //load page number from eeprom
    //    if (lastRXpage_eeprom < 1) {
    //      lastRXpage_eeprom = 0;
    //    }


    //wait for time to sync from NTP server
    while (time(nullptr) <= 100000) {
      delay(20);
    }

    time_t now = time(nullptr);   //get current time
    struct tm * timeinfo;
    time(&now);
    timeinfo = localtime(&now);

    Serial.print("Unix epoch time is: ");
    Serial.println(now);
    char formattedTime [30];
    strftime (formattedTime, 30, "%r", timeinfo); // http://www.cplusplus.com/reference/ctime/strftime/
    Serial.print("Regular time is: ");
    Serial.println(formattedTime);


  //--------------------------------------------
  // Get Telegram Bot account info
  Bot.getMe();

  //--------------------------------------------
  // Set Telegram Bot /Start message

  // Send a Telegram message for start
  delay(100);
  Bot.sendMessage(telegram_chat_id, TEXT_START);



    
  }

  else {

  }

}
