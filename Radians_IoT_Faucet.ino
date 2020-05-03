/********************************************************************************
    Radians Ark.io BridgeChain IoT Faucet
    This projects runs on an ESP8266 or ESP32 microcontroller and provides a way to easily send Ark Bridgechain Cryptocurrency to a wallet address that is sent via Telegram app.
    This faucet is intended to provide a simple way to distribute free tokens on a testnet during development.

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
// Standard C/C++ libraries
#include <string.h>

// Device libraries (Arduino ESP32/ESP8266 Cores)
#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
// Custom libraries
#include <utlgbotlib.h>

/**************************************************************************************************/

// WiFi Parameters
#define WIFI_SSID "TELUS0183"
#define WIFI_PASS "6z5g4hbdxi"

#define MAX_CONN_FAIL 50
#define MAX_LENGTH_WIFI_SSID 31
#define MAX_LENGTH_WIFI_PASS 63

// Telegram Bot Token (Get from Botfather)
#define TLG_TOKEN "833803898:AAG9mcKAEzdd7W7p_RtjIZqp48Lt4X-tTMw"
#define telegram_chat_id "-1001491687302"  //Add @RawDataBot to your group chat to find the chat id.


// Enable Bot debug level (0 - None; 1 - Bot Level; 2 - Bot+HTTPS Level)
#define DEBUG_LEVEL_UTLGBOT 0

// Board Led Pin
#define PIN_LED 13

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
  "\n"
  "/start - Show start text.\n"
  "/help - Show actual text.\n"
  "/ledon - Turn on the LED.\n"
  "/ledoff - Turn off the LED.\n"
  "/ledstatus - Show actual LED status.\n"
  "/address - Returns wallet address of faucet.\n"
  "/balance - Returns balance of wallet.\n"
  "/request\\_WALLETADDRESS : replace WALLETADDRESS with your address";


/**************************************************************************************************/

/* Functions Prototypes */

void wifi_init_stat(void);
bool wifi_handle_connection(void);

/**************************************************************************************************/

/* Globals */

// Create Bot object
uTLGBot Bot(TLG_TOKEN);

// LED status
uint8_t led_status;

/**************************************************************************************************/


/**************************************************************************************************/
// blockchain stuff
const char* FaucetAddress = "TEf7p5jf1LReywuits5orBsmpkMe8fLTkk";   //RADIANS testnet address that holds faucet funds
static const auto PASSPHRASE  = "canal pride surprise flat twist genre access weekend wonder task charge whale";
const char* ARK_PEER = "37.34.60.90";  //RADIANS Testnet Peer
int ARK_PORT = 4040;
#include <arkCrypto.h>
#include "arkCrypto_esp32.h"  // This is a helper header that includes all the Misc ARK C++ Crypto headers required for this sketch
#include "transactions/builders/radians/radians.hpp"

// Namespaces
using namespace Ark::Crypto;
using namespace Ark::Crypto::identities;
using namespace Ark::Crypto::transactions;

// Configure Bridgechain Parameters
static const auto BRIDGECHAIN_NETHASH   = "f39a61f04d6136a690a0b675ef6eedbd053665bd343b4e4f03311f12065fb875"; // std::string
static const auto BRIDGECHAIN_SLIP44    = 1;          // uint8_t
static const auto BRIDGECHAIN_WIF       = 0xCE;       // uint8_t
static const auto BRIDGECHAIN_VERSION   = 0x41;       // uint8_t
static const auto BRIDGECHAIN_EPOCH     = "2019-10-25T09:05:40.856Z";  // std::string

// BridgeChain Network Structure Model.  see Ark-Cpp-Crypto\src\common\network.hpp
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
  Ark Client Library (version 1.4.0)
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
          

/**************************************************************************************************/


/* Main Function */

void setup(void)
{
  // Enable Bot debug
  Bot.set_debug(DEBUG_LEVEL_UTLGBOT);

  // Initialize Serial
  Serial.begin(115200);

  // Initialize LED pin as digital output
  digitalWrite(PIN_LED, LOW);
  pinMode(PIN_LED, OUTPUT);
  led_status = 0;

  // Initialize WiFi station connection
  wifi_init_stat();

  // Wait for WiFi connection
  Serial.println("Waiting for WiFi connection.");
  while (!wifi_handle_connection())
  {
    Serial.print(".");
    delay(500);
  }

  // Get Bot account info
  Bot.getMe();
  // Send a Telegram message for start
  delay(100);
  Bot.sendMessage(telegram_chat_id, TEXT_START);


}

void loop()
{
  // Check if WiFi is connected
  if (!wifi_handle_connection())
  {
    // Wait 100ms and check again
    delay(100);
    return;
  }

  // Check for Bot received messages
  while (Bot.getUpdates())
  {
    // Show received message text
    Serial.println("");
    Serial.println("Received message:");
    Serial.println(Bot.received_msg.text);
    Serial.println("");


    //--------------------------------------------
    // If /start command was received
    if (strncmp(Bot.received_msg.text, "/start", strlen("/start")) == 0)
    {
      // Send a Telegram message for start
      Bot.sendMessage(Bot.received_msg.chat.id, TEXT_START);
    }


    //--------------------------------------------
    // If /help command was received
    else if (strncmp(Bot.received_msg.text, "/help", strlen("/help")) == 0)
    {
      // Send a Telegram message for start
      Bot.sendMessage(Bot.received_msg.chat.id, TEXT_HELP);
    }


    //--------------------------------------------
    // If /ledon command was received
    else if (strncmp(Bot.received_msg.text, "/ledon", strlen("/ledon")) == 0)
    {
      // Turn on LED
      led_status = 1;
      digitalWrite(PIN_LED, HIGH);

      // Show command reception through Serial
      Serial.println("Command /ledon received.");
      Serial.println("Turning on the LED.");

      // Send a Telegram message to notify that the LED has been turned on
      Bot.sendMessage(Bot.received_msg.chat.id, "Led turned on.");
    }


    //--------------------------------------------
    // If /ledoff command was received
    else if (strncmp(Bot.received_msg.text, "/ledoff", strlen("/ledoff")) == 0)
    {
      // Turn off LED
      led_status = 0;
      digitalWrite(PIN_LED, LOW);

      // Show command reception through Serial
      Serial.println("Command /ledoff received.");
      Serial.println("Turning off the LED.");

      // Send a Telegram message to notify that the LED has been turned off
      Bot.sendMessage(Bot.received_msg.chat.id, "Led turned off.");
    }


    //--------------------------------------------
    // If /ledstatus command was received
    else if (strncmp(Bot.received_msg.text, "/ledstatus", strlen("/ledstatus")) == 0)
    {
      // Send a Telegram message to notify actual LED status
      if (led_status)
        Bot.sendMessage(Bot.received_msg.chat.id, "The LED is on.");
      else
        Bot.sendMessage(Bot.received_msg.chat.id, "The LED is off.");
    }


    //--------------------------------------------
    // If /address command was received
    else if (strncmp(Bot.received_msg.text, "/address", strlen("/address")) == 0)
    {
        Bot.sendMessage(Bot.received_msg.chat.id, FaucetAddress);
    }



    // Feed the Watchdog
    yield();
  }

  // Wait 1s for next iteration
  delay(1000);
}

/**************************************************************************************************/

/* Functions */

// Init WiFi interface
void wifi_init_stat(void)
{
  Serial.println("Initializing TCP-IP adapter...");
  Serial.print("Wifi connecting to SSID: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("TCP-IP adapter successfuly initialized.");
}

/**************************************************************************************************/

/* WiFi Change Event Handler */

bool wifi_handle_connection(void)
{
  static bool wifi_connected = false;

  // Device is not connected
  if (WiFi.status() != WL_CONNECTED)
  {
    // Was connected
    if (wifi_connected)
    {
      Serial.println("WiFi disconnected.");
      wifi_connected = false;
    }

    return false;
  }
  // Device connected
  else
  {
    // Wasn't connected
    if (!wifi_connected)
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());

      wifi_connected = true;
    }

    return true;
  }
}

/**************************************************************************************************/
