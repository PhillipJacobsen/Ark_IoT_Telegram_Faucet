//h
#define WIFI_SSID         "TELUS0183"
#define WIFI_PASS         "6z5g4hbdxi"

//w
//#define WIFI_SSID         "TELUS6428"
//#define WIFI_PASS         "3mmkgc9gn2"

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
int16_t DST = 0;              //To enable Daylight saving time set it to 3600. Otherwise, set it to 0. This does not seem to work!!

// Configure Radians Wallet for Faucet
const char* FaucetAddress = "TCpoh6TYY8KZnwamGgmgTRcp5k9ycumH83";   //RADIANS testnet address that holds faucet funds
static const auto PASSPHRASE  = "often power release space funny lend mango blood pet ribbon scene what";

//Configure the quantity of RAD sent per faucet request.  
//const char* PAYOUT_AMOUNT = "100000000";                    
constexpr uint64_t PAYOUT_AMOUNT_UINT64 = 100000000;     //default: 1000000000 = 10 RAD
const char* PAYOUT_MESSAGE = "Telegram Faucet";     


//Configure Bridgechain Relay
const char* ARK_PEER = "37.34.60.90";  //RADIANS Testnet Peer
int ARK_PORT = 4040;

// Configure Bridgechain Parameters
static const auto BRIDGECHAIN_NETHASH   = "f39a61f04d6136a690a0b675ef6eedbd053665bd343b4e4f03311f12065fb875"; // std::string
static const auto BRIDGECHAIN_SLIP44    = 1;          // uint8_t
static const auto BRIDGECHAIN_WIF       = 0xCE;       // uint8_t
static const auto BRIDGECHAIN_VERSION   = 0x41;       // uint8_t
static const auto BRIDGECHAIN_EPOCH     = "2019-10-25T09:05:40.856Z";  // std::string
constexpr uint64_t TYPE_0_FEE           = 1000000ULL;   //0.01RAD
constexpr uint8_t TYPE_0_TYPE           = 0U;



//--------------------------------------------
// If you are reprogramming a new wallet address into an existing ESP32 module you need to erase the Flash which stores the number of received transactions in the wallet.
// 1. Define ERASE_FLASH below
// 2. Download firmware
// 3. undefine ERASE_FLASH and reprogram
//#define ERASE_FLASH
