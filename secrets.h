
#define MQTT_SERVER_IP    "40.85.223.207"
#define MQTT_USERNAME     "MQTTFaucet"
#define MQTT_PASSWORD     "MagpieCrow77"
//#define MQTT_CLIENT_NAME  "TestClient"
char* MQTT_CLIENT_NAME =  "TestClient";
#define MQTT_SERVER_PORT  1883

//h
#define WIFI_SSID         "TELUS0357"
#define WIFI_PASS         "77kmm7r7hz"

//w
//#define WIFI_SSID         "TELUS6428"
//#define WIFI_PASS         "3mmkgc9gn2"

//#define _ENABLE_TG_BOT
//#define _ENABLE_MQTT_BOT

//configure Telegram
// Telegram Bot Token (Get from Botfather)
#define TLG_TOKEN "833803898:AAG9mcKAEzdd7W7p_RtjIZqp48Lt4X-tTMw"
#define telegram_chat_id "-1001491687302"   //Add @RawDataBot to your group chat to find the chat id.

// Enable Bot debug level (0 - None; 1 - Bot Level; 2 - Bot+HTTPS Level)
#define DEBUG_LEVEL_UTLGBOT 2
#define UTLGBOT_MEMORY_LEVEL 4
//-DUTLGBOT_MEMORY_LEVEL=0 // Max TLG msgs:  128 chars
//-DUTLGBOT_MEMORY_LEVEL=1 // Max TLG msgs:  256 chars
//-DUTLGBOT_MEMORY_LEVEL=2 // Max TLG msgs:  512 chars
//-DUTLGBOT_MEMORY_LEVEL=3 // Max TLG msgs: 1024 chars
//-DUTLGBOT_MEMORY_LEVEL=4 // Max TLG msgs: 2048 chars
//-DUTLGBOT_MEMORY_LEVEL=5 // Max TLG msgs: 4097 chars (telegram max msg length)

// from tlgcert.h
// Note: This certificate will expire 23/05/2020. After that date, Telegram will get a new one so
// you must check for it and change it 


//int8_t TIME_ZONE = -6;      //set timezone:  MST (use this in summer)
int8_t TIME_ZONE = -7;        //set timezone:  MST (use this in winter)
int16_t DST = 3600;           //To enable Daylight saving time set it to 3600. Otherwise, set it to 0. I am not sure if this actually works. Need to confirm in the fall...

// Configure Radians Wallet for Faucet
const char* FaucetAddress = "TQz8RicTCZdDVNg8AV289VAuL28FGezhxE";   //RADIANS testnet address that holds faucet funds
static const auto PASSPHRASE  = "expire faint loop traffic stay aspect payment second snap first skull word";

//Configure the quantity of RAD sent per faucet request.  
//const char* PAYOUT_AMOUNT = "1000";                    
constexpr uint64_t PAYOUT_AMOUNT_UINT64 = 1000;    
const char* PAYOUT_MESSAGE = "Telegram Faucet: Username ";     
const char* PAYOUT_MESSAGE_MQTT = "MQTT Faucet";   

//Configure Bridgechain Relay
//const char* ARK_PEER = "37.34.60.90";  //RADIANS Testnet Peer
const char* ARK_PEER = "138.197.165.189";  //RADIANS Testnet Peer - PJ's

int ARK_PORT = 4040;

// Configure Bridgechain Parameters
static const auto BRIDGECHAIN_NETHASH   = "314ccfc8c437e10cccb527ee6726be606da8fbaebe54c5c105df30882511c25a"; // std::string
static const auto BRIDGECHAIN_SLIP44    = 1;          // uint8_t
static const auto BRIDGECHAIN_WIF       = 0x91;       // uint8_t      145
static const auto BRIDGECHAIN_VERSION   = 0x41;       // uint8_t      65
static const auto BRIDGECHAIN_EPOCH     = "2020-05-12T11:34:19.156Z";  // std::string
