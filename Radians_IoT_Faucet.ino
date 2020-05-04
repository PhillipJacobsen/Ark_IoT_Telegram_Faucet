/********************************************************************************
    Radians Ark.io BridgeChain IoT Faucet
    This projects runs on an ESP8266 or ESP32 microcontroller and provides a way to easily send Ark Bridgechain Cryptocurrency to a wallet address that is sent via Telegram app.

    Radians_IoT_faucet.ino
    2020 @phillipjacobsen

    Program Features:
    This program implements a Telegram BOT running on an ESP32 microcontroller that can dispense Ark.io blockchain tokens upon request.

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
const int LED_PIN = 13;     //LED integrated on Adafruit HUZZAH32 module
const int BAT_PIN = 35;     //ADC connected to Battery input pin (A13 = 35;)
//const int DAC1 = 25;      //declared in \packages\esp32\hardware\esp32\1.0.4\variants\feather_esp32/pins_arduino.h
//const int DAC2 = 26;


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

// Create Bot object
uTLGBot Bot(TLG_TOKEN);

/**************************************************************************************************/
//WiFi Parameters required by Telegram Library
#define MAX_CONN_FAIL 50
#define MAX_LENGTH_WIFI_SSID 31
#define MAX_LENGTH_WIFI_PASS 63


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
uint32_t UpdateInterval_TelegramBot = 1000;           // 1000ms
uint32_t previousUpdateTime_TelegramBot = millis();



  

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
void wifi_init_stat(void);
bool wifi_handle_connection(void);





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
