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

  //--------------------------------------------
  // Initialize WiFi Station connection
  wifi_init_stat();

  // Wait for WiFi connection
  Serial.println("Waiting for WiFi connection.");
  while (!wifi_handle_connection())
  {
    Serial.print(".");
    delay(500);
  }

  //--------------------------------------------
  //  sync local time to NTP server
  configTime(TIME_ZONE * 3600, DST, "pool.ntp.org", "time.nist.gov");

  //--------------------------------------------
  //  display IP address.
  Serial.println("\nIP address: ");
  Serial.println(WiFi.localIP());


  //--------------------------------------------
  // Get Telegram Bot account info
  Bot.getMe();

  //--------------------------------------------
  // Set Telegram Bot /Start message

  // Send a Telegram message for start
  delay(100);
  Bot.sendMessage(telegram_chat_id, TEXT_START);


  //wait for time to sync from NTP server
  while (time(nullptr) <= 100000) {
    delay(20);
  }

  time_t now = time(nullptr);       //get current time
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now);

  Serial.print("Unix epoch time is: ");
  Serial.println(now);
  char formattedTime [30];
  strftime (formattedTime, 30, "%r", timeinfo); // http://www.cplusplus.com/reference/ctime/strftime/
  Serial.print("Regular time is: ");
  Serial.println(formattedTime);
}
