/********************************************************************************
    Radians Ark.io BridgeChain IoT Faucet
    This projects runs on an ESP8266 or ESP32 microcontroller and provides a way to easily send Ark Bridgechain Cryptocurrency to a wallet address that is sent via Telegram app.

    Radians_IoT_faucet.ino
    2020 @phillipjacobsen

    Program Features:
    This program has been tested with ESP32 however it should also work with ESP8266 modules with minor changes to hardware connections and wifi libraries.
    This sketch uses the ARK Cpp-Client API to interact with an Ark V2.6 Bridgechain Testnet/Devnet node.

    This program will provide similar features/settings as the ARK devnet faucet for DARK coins by deadlock-delegate
    https://github.com/deadlock-delegate/faucet

    faucetAddress - address of your faucet
    vendorField - what message you wish to add to each payout
    payoutAmount - what amount you wish to payout per request
    payoutCooldown - for how long will user not be able to request a payment for
    dailyPayoutLimit - what's the faucet's max (overall) daily payout
********************************************************************************/

/********************************************************************************
                              Private Data & Configuration
  IMPORTANT - Modify the secrets.h file with your network connection and configuration details
********************************************************************************/
#include "secrets.h"

/********************************************************************************
                            Misc I/O Definitions
********************************************************************************/
const int LED_PIN = 13;     //LED integrated on Adafruit HUZZAH32 module
const int BAT_PIN = 35;     //ADC connected to Battery input pin (A13 = 35;)
//const int DAC1 = 25;      //declared in \packages\esp32\hardware\esp32\1.0.4\variants\feather_esp32/pins_arduino.h
//const int DAC2 = 26;

// Standard C/C++ libraries
//#include <string.h>

// Device libraries (Arduino ESP32/ESP8266 Cores)
//#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif


/********************************************************************************
    utlgbotlib Telegram bot Library by JRios Version 1.0.0
    https://github.com/J-Rios/uTLGBotLib-arduino
    
********************************************************************************/
#include <utlgbotlib.h>

/**************************************************************************************************/
//WiFi Parameters required by Telegram Library
#define MAX_CONN_FAIL 50
#define MAX_LENGTH_WIFI_SSID 31
#define MAX_LENGTH_WIFI_PASS 63



/********************************************************************************
  Time Library
  required for internal clock to syncronize with NTP server.
********************************************************************************/
#include "time.h"


/********************************************************************************
  Update Intervals for various algorithms
********************************************************************************/
uint32_t UpdateInterval_TelegramBot = 1000;           // 1000ms
uint32_t previousUpdateTime_TelegramBot = millis();

// Telegram Bot /start text message


const char TEXT_START[] =
  "Radians.nl Ark BridgeChain IoT Faucet Ready.\n"
  "I am a Bot running in an ESP microcontroller.\n"
  "I can send you free RAD coins.\n"
  "\n"
  "Use /help command to see how to use me.";

// Telegram Bot /help text message
const char TEXT_HELP[] =
  "Available Commands:\n"
  "/start - Show start text.\n"
  "/help - Show actual text.\n"
//  "/ledon - Turn on the LED.\n"
//  "/ledoff - Turn off the LED.\n"
//  "/ledstatus - Show actual LED status.\n"
  "/address - Returns wallet address of faucet.\n"
  "/balance - Returns balance of faucet wallet.\n"
  "/request\\_WALLETADDRESS : replace WALLETADDRESS with your address.\n"
  "/mybalance\\_MYBALANCE : replace MYBALANCE with your address.";


/********************************************************************************
  Function prototypes
  Arduino IDE normally does its automagic here and creates all the function prototypes for you.
  We have put functions in other files so we need to manually add some prototypes as the automagic doesn't work correctly
********************************************************************************/
void wifi_init_stat(void);
bool wifi_handle_connection(void);


/* Globals */

// Create Bot object
uTLGBot Bot(TLG_TOKEN);

// LED status
uint8_t led_status;

/**************************************************************************************************/


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
  MAIN LOOP
********************************************************************************/
void loop()
{
  //--------------------------------------------
  // Check if WiFi is connected
  if (!wifi_handle_connection())
  {
    // Wait 100ms and check again
    delay(100);
    return;
  }

  //--------------------------------------------
  // Check for Telegram Bot received messages
  if (millis() - previousUpdateTime_TelegramBot > UpdateInterval_TelegramBot)  {
    previousUpdateTime_TelegramBot += UpdateInterval_TelegramBot;

    telegramBotHandler();
  }

}
