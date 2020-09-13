/********************************************************************************
    Ark.io BridgeChain IoT Telegram and MQTT Faucet
    This projects runs on an ESP32 microcontroller and provides a way to easily send Ark Bridgechain Cryptocurrency to a wallet address that is sent via Telegram app
    or via MQTT.

    Ark_Monitor_WiFi_Lora.ino
    2020 @phillipjacobsen

    Program Features:
    This program implements a Telegram BOT running on an ESP32 microcontroller that can dispense Ark.io custom blockchain tokens upon request.
    Default settings are to operate with custom Radians blockchain

    FaucetAddress - address of your Radians bridgechain token faucet
    PAYOUT_MESSAGE - message you wish to add to each payout
    PAYOUT_AMOUNT_UINT64 - amount that is sent per reuest
********************************************************************************/

/********************************************************************************
                              Private Data & Configuration
  IMPORTANT - Modify the secrets.h file with your network connection and configuration details
********************************************************************************/
#include "secrets.h"

/********************************************************************************
                            Misc I/O Definitions
********************************************************************************/
//const int LED_PIN = 13;   //LED integrated on Adafruit HUZZAH32 module
const int LED_PIN = 25;     //LED integrated on Heltec WiFi Kit
//const int BAT_PIN = 35;     //ADC connected to Battery input pin (A13 = 35;)


#include <FastLED.h>

#define NUM_LEDS 16
#define DATA_PIN 13
// Define the array of leds
CRGB leds[NUM_LEDS];

bool New_Block_Event = false;   //used to detect first run after power up
byte LED_Fade_Brightness = 0;
byte masterBright_ceiling = 254;
byte New_Block_Event_FadeState = 0;   // 0 means fade in, 1 means fade out

const uint8_t PROGMEM gamma8[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
  2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
  5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};



/********************************************************************************
                              Various Global Variables
********************************************************************************/
bool initialConnectionEstablished_Flag = false;   //used to detect first run after power up


/********************************************************************************
    OLED display

********************************************************************************/
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C   u8g2(U8G2_R0, 16, 15, 4);     //   reset,scl,sda     for Heltec WiFi 32 and Heltec WiFi Lora 32

/********************************************************************************
    utlgbotlib Telegram bot Library by JRios Version 1.0.0
    https://github.com/J-Rios/uTLGBotLib-arduino

********************************************************************************/
#include <utlgbotlib.h>

// Create Bot object
uTLGBot Bot(TLG_TOKEN);

/**************************************************************************************************/
//WiFi Parameters required by Telegram Library
#define MAX_CONN_FAIL 50
#define MAX_LENGTH_WIFI_SSID 31
#define MAX_LENGTH_WIFI_PASS 63

//Changed this in utlgbotlib.h
// Telegram getUpdate Long Poll value (s)
//#define TELEGRAM_LONG_POLL 5      //default is 10

/*
  after bot has been created use the following in BotFather to change the list of commands supported by your bot.
  Users will see these commands as suggestions when they type / in the chat with your bot.

  /setcommands
  then enter commands like this. all in one chat. It seems you have to add all commands at once. I am not sure how to just add a new command to the list.
  start - Show startup message
  help - Show list of commands
  time - Show options keyboard
  name - Get Bot name
  time - Get current Time
  balance - Get balance of faucet wallet
  address - Get address of faucet

  request_walletaddress - Request tokens
  mybalance_walletaddress - get your balance
*/

/********************************************************************************
  Time Library
  required for internal clock to syncronize with NTP server.
********************************************************************************/
#include "time.h"


/********************************************************************************
  Update Intervals for various algorithms
********************************************************************************/
uint32_t UpdateInterval_TelegramBot = 5100;
uint32_t previousUpdateTime_TelegramBot = millis();

uint32_t UpdateInterval_LEDfade = 13;
uint32_t previousUpdateTime_LEDfade = millis();

/********************************************************************************
   Arduino Json Libary - Tested with version 6.13

********************************************************************************/
//#include <ArduinoJson.h>


/********************************************************************************
  Library for reading/writing to the ESP32 flash memory.
  ESP32 Arduino libraries emulate EEPROM using a sector (4 kilobytes) of flash memory.
  The total flash memory size is ???
  The entire space is split between bootloader, application, OTA data, NVS, SPIFFS, and EEPROM.
  EEPROM library on the ESP32 allows using at most 1 sector (4kB) of flash.
********************************************************************************/
#include <EEPROM.h>

/********************************************************************************
    EspMQTTClient Library by @plapointe6 Version 1.8.0
    WiFi and MQTT connection handler for ESP32
    This library does a nice job of encapsulating the handling of WiFi and MQTT connections.
    https://github.com/plapointe6/EspMQTTClient
    You just need to provide your credentials and it will manage the connection and reconnections to the Wifi and MQTT networks.
    EspMQTTClient is a wrapper around the MQTT PubSubClient Library Version 2.7 by @knolleary
********************************************************************************/
// The MQTT packets are larger then the allowed for the default setting of the libary.
// You need to update this line in PubSubClient.h. Setting it here does nothing.
// If you update this library you will need to update this setting as it will be overwritten.
// #define MQTT_MAX_PACKET_SIZE 512  // the maximum message size, including header, is 128 bytes by default. Configurable in \Arduino\libraries\PubSubClient\src\PubSubClient.h.

#include "EspMQTTClient.h"


EspMQTTClient WiFiMQTTclient(
  WIFI_SSID,
  WIFI_PASS,
  MQTT_SERVER_IP,   // MQTT Broker server ip
  MQTT_USERNAME,    // Can be omitted if not needed
  MQTT_PASSWORD,    // Can be omitted if not needed
  MQTT_CLIENT_NAME, // Client name that uniquely identify your device
  MQTT_SERVER_PORT  // The MQTT port, default to 1883. this line can be omitted
);



/********************************************************************************
  State Machine
********************************************************************************/
enum State_enum {STATE_0, STATE_1, STATE_2, STATE_3, STATE_4, STATE_5, STATE_6};    //The possible states of the state machine
State_enum state = STATE_0;     //initialize the starting state.

/********************************************************************************
  Update Intervals for various algorithms
********************************************************************************/
//Frequency at which the MQTT packets are published
uint32_t UpdateInterval_MQTT_Publish = 10000;           // 10 seconds
uint32_t previousUpdateTime_MQTT_Publish = millis();

// Telegram Bot /start text message
const char TEXT_START[] =
  "Radians.nl Ark BridgeChain IoT Faucet Ready.\n"
  "I am a Bot running in an ESP microcontroller.\n"
  "I can send you free RAD coins.\n"
  "\n"
  "Use /help command to see how to use me.";

// Telegram Bot /help text message
const char TEXT_HELP[] =
  "Ark.io BridgeChain Token Faucet Bot.\n"
  "I am a Bot running in a microcontroller.\n"
  "I can send you free tokens for the Radians.nl blockchain.\n\n"
  "Available Commands:\n"
  // "/start - Show Bot startup message .\n"
  "/help - Show available commands.\n"
  "/time - Returns the time.\n"
  //  "/ledon - Turn on the LED.\n"
  //  "/ledoff - Turn off the LED.\n"
  //  "/ledstatus - Show actual LED status.\n"
  "/address - Returns wallet address of faucet.\n"
  "/balance - Returns balance of faucet wallet.\n"
  "/request\\_WALLETADDRESS : Sends RAD tokens to your address.\n    Replace WALLETADDRESS with your address.\n"
  "/mybalance\\_MYBALANCE : Returns balance of your wallet.\n    Replace MYBALANCE with your address.";


/********************************************************************************
  Function prototypes
  Arduino IDE normally does its automagic here and creates all the function prototypes for you.
  We have put functions in other files so we need to manually add some prototypes as the automagic doesn't work correctly
********************************************************************************/



// LED status
uint8_t led_status;



/**************************************************************************************************/

#include <arkCrypto.h>
#include "arkCrypto_esp32.h"  // This is a helper header that includes all the Misc ARK C++ Crypto headers required for this sketch
#include "transactions/builders/radians/radians.hpp"

// Namespaces
using namespace Ark::Crypto;
using namespace Ark::Crypto::identities;
using namespace Ark::Crypto::transactions;


// BridgeChain Network Structure Model.
// These are defined in secrets.h
const Network BridgechainNetwork = {
  BRIDGECHAIN_NETHASH,
  BRIDGECHAIN_SLIP44,
  BRIDGECHAIN_WIF,
  BRIDGECHAIN_VERSION,
  BRIDGECHAIN_EPOCH
};

// Load the Custom Network Configuration
const Configuration cfg(BridgechainNetwork);


/********************************************************************************
  Ark Client Library (version 1.4.1)
********************************************************************************/
#include <arkClient.h>
Ark::Client::Connection<Ark::Client::Api> connection(ARK_PEER, ARK_PORT);   // create ARK blockchain connection



/********************************************************************************
  This structure is used to store details of the bridgechain wallet
********************************************************************************/
struct wallet {
  char walletBalance[64 + 1];             //current balance
  uint64_t walletBalance_Uint64 = 0ULL;   //current balance
  char walletNonce[64 + 1];               //current nonce
  uint64_t walletNonce_Uint64 = 1ULL;     //current nonce
};
struct wallet bridgechainWallet;

char receiveaddress_char[34 + 1];

char mybalanceAddress_char[34 + 1];
char mywalletBalance[64 + 1];             //current balance
uint64_t mywalletBalance_Uint64 = 0ULL;   //current balance









/********************************************************************************
  Function prototypes
  Arduino IDE normally does its automagic here and creates all the function prototypes for you.
  We have put functions in other files so we need to manually add some prototypes as the automagic doesn't work correctly
********************************************************************************/
void StateMachine();
//void send_MQTTpacket();


/********************************************************************************
  MAIN LOOP
********************************************************************************/
void loop() {

  //--------------------------------------------
  // Process state machine
  StateMachine();

  //--------------------------------------------
  // Handle the WiFi and MQTT connections
  WiFiMQTTclient.loop();

  //--------------------------------------------
  // Publish MQTT data every UpdateInterval_MQTT_Publish (10 seconds)
  //  send_MQTTpacket();

  if (New_Block_Event) {     //check for new block event
    if (millis() - previousUpdateTime_LEDfade > UpdateInterval_LEDfade)  {
      previousUpdateTime_LEDfade += UpdateInterval_LEDfade;

      //fades in and then out
      //      LED_Fade_Brightness = LED_Fade_Brightness + gFadeAmount;
      //      if (LED_Fade_Brightness == 0 || LED_Fade_Brightness > 254) {
      //        gFadeAmount = -gFadeAmount;
      //      }
      //      FastLED.setBrightness(gamma8[LED_Fade_Brightness]);

      //logarithmic fade in
      //      LED_Fade_Brightness = LED_Fade_Brightness + gFadeAmount;
      //      if (LED_FLED_Fade_Brightness > 254) {
      //        gFadeAmount = -gFadeAmount;
      //      }
      //      FastLED.setBrightness(gamma8[LED_Fade_Brightness]);
      //      FastLED.show();

  
      switch (New_Block_Event_FadeState) {
        case 0:
          //simple fade in
          FastLED.setBrightness(gamma8[LED_Fade_Brightness]);
          FastLED.show();
          if (LED_Fade_Brightness++ > masterBright_ceiling)
          {
            LED_Fade_Brightness = masterBright_ceiling;
            //  LED_Fade_Brightness = 0;
            //  FastLED.setBrightness(LED_Fade_Brightness);
            //  FastLED.show();
            New_Block_Event_FadeState = 1;
            //   New_Block_Event = false;
          }
          break;

        case 1:
          //simple fade out
          FastLED.setBrightness(gamma8[LED_Fade_Brightness]);
          FastLED.show();
          if (LED_Fade_Brightness-- < 1)
          {
            LED_Fade_Brightness = 0;
            FastLED.setBrightness(LED_Fade_Brightness);
            FastLED.show();
            New_Block_Event_FadeState = 0;
            New_Block_Event = false;
          }
          break;
      }




    }





    //    leds[7] = CRGB::Blue;
    //    FastLED.show();
    //    delay(200);
    //    leds[7] = CRGB::Black;
    //    FastLED.show();

  }

  //--------------------------------------------
  // Check for Telegram Bot received messages

  //  if (initialConnectionEstablished_Flag) {
  //    if (millis() - previousUpdateTime_TelegramBot > UpdateInterval_TelegramBot)  {
  //      previousUpdateTime_TelegramBot += UpdateInterval_TelegramBot;
  //      telegramBotHandler();
  //    }
  //  }


}
