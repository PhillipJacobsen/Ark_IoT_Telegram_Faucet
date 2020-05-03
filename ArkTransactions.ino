/********************************************************************************
  This file contains functions that interact with Ark client C++ API
********************************************************************************/


/********************************************************************************
  This routine checks to see if Ark node is syncronized to the chain.
  This is a maybe a good way to see if node communication is working correctly.
  This might be a good routine to run periodically
  Returns True if node is synced

     The following method can be used to get the Status of a Node.
     This is equivalant to calling '167.114.29.49:4003/api/v2/node/status'
     json-formatted object:
  {
   "data":{
      "synced":true,
      "now":4047140,
      "blocksCount":-4047140,
      "timestamp":82303508
   }
  }

    https://arduinojson.org/v6/api/jsondocument/
    https://arduinojson.org/v6/assistant/
********************************************************************************/
bool checkArkNodeStatus() {
  Serial.println("\n=================================");
  Serial.println("Check status of Radians Relay Node");

  const auto nodeStatus = connection.api.node.status();   //get status of Ark node
  Serial.print("\nNode Status: ");
  Serial.println(nodeStatus.c_str()); // The response is a 'std::string', to Print on Arduino, we need the c_string type.

  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(4) + 50;
  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, nodeStatus.c_str());
  JsonObject data = doc["data"];
  return data["synced"];
}


/********************************************************************************
  This routine retrieves the current nonce and the balance for the wallet

     This is equivalant to calling http://37.34.60.90:4040/api/v2/wallets/TRXA2NUACckkYwWnS9JRkATQA453ukAcD1

     json-formatted object:
  {
     "data":{
        "address":"TRXA2NUACckkYwWnS9JRkATQA453ukAcD1",
        "publicKey":"03e063f436ccfa3dfa9e9e6ee5e08a65a82a5ce2b2daf58a9be235753a971411e2",
        "nonce":"140",
        "balance":"94968174556",
        "isDelegate":false,
        "isResigned":false
     }
  }

  this function writes directly to these global variables:
  struct wallet {
  char walletBalance[64 + 1];
  uint64_t walletNonce_Uint64 = 1ULL;
  char walletNonce[64 + 1];
  uint64_t walletBalance_Uint64 = 0ULL;
  }

********************************************************************************/
void getWallet() {
  const auto walletGetResponse = connection.api.wallets.get(FaucetAddress);

  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(6) + 200;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, walletGetResponse.c_str());
  JsonObject data = doc["data"];
  strcpy(bridgechainWallet.walletBalance, data["balance"]);                     //copy into global character array
  bridgechainWallet.walletBalance_Uint64 = strtoull(data["balance"], NULL, 10); //convert string to unsigned long long

  strcpy(bridgechainWallet.walletNonce, data["nonce"]);                         //copy into global character array
  bridgechainWallet.walletNonce_Uint64 = strtoull(data["nonce"], NULL, 10);     //convert string to unsigned long long

  Serial.println("\n=================================");
  Serial.println("Retrieving wallet Nonce & Balance");
  Serial.print("Get Wallet Response");
  Serial.println(walletGetResponse.c_str());                                    // The response is a 'std::string', to Print on Arduino, we need the c_string type.
  Serial.print("Nonce: ");
  //Serial.println(bridgechainWallet.walletNonce);                              // serial.print does not have support for Uint64
  Serial.printf("%" PRIu64 "\n", bridgechainWallet.walletNonce_Uint64);         // PRIx64 to print in hexadecimal
  Serial.print("Balance: ");
  Serial.println(bridgechainWallet.walletBalance);
}







/********************************************************************************
  Send a standard BridgeChain transaction, tailored for a custom network.

  NOTE: This project does not send any standard transactions so the following is not used.

********************************************************************************/


void sendBridgechainTransaction() {
  // Use the Transaction Builder to make a transaction.
  bridgechainWallet.walletNonce_Uint64 = bridgechainWallet.walletNonce_Uint64 + 1;

  char tempVendorField[80];
  strcpy(tempVendorField, "Telegram Faucet");
  //  strcat(tempVendorField, scooterRental.sessionID_QRcode);

  auto bridgechainTransaction = builder::Transfer(cfg)
                                .recipientId(receiveaddress_char)
                                .vendorField(tempVendorField)
                                .fee(1000000)
                                .nonce(bridgechainWallet.walletNonce_Uint64)
                                .amount(1000000000)
                                .sign(PASSPHRASE)
                                .build();

  const auto transactionJson = bridgechainTransaction.toJson();
  printf("\n\nBridgechain Transaction: %s\n\n", transactionJson.c_str());

  bridgechainTransaction.sign(PASSPHRASE);

  char transactionsBuffer[1500];
  snprintf(&transactionsBuffer[0], 1500, "{\"transactions\":[%s]}", bridgechainTransaction.toJson().c_str());
  std::string jsonStr = transactionsBuffer;
  std::string sendResponse = connection.api.transactions.send(jsonStr);
  Serial.println(sendResponse.c_str());

  const size_t capacity = 2 * JSON_ARRAY_SIZE(0) + 2 * JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(4) + 190;
  DynamicJsonDocument doc(capacity);
  //const char* json = "{\"data\":{\"accept\":[\"bd0f614f1de28788d048ac3d289878aa0297dbf6e8ebf5fbfc49c316983aa5f2\"],\"broadcast\":[\"bd0f614f1de28788d048ac3d289878aa0297dbf6e8ebf5fbfc49c316983aa5f2\"],\"excess\":[],\"invalid\":[]}}";
  deserializeJson(doc, sendResponse.c_str());
  JsonObject data = doc["data"];
  const char* data_accept_0 = data["accept"][0]; // "bd0f614f1de28788d048ac3d289878aa0297dbf6e8ebf5fbfc49c316983aa5f2"
  const char* data_broadcast_0 = data["broadcast"][0]; // "bd0f614f1de28788d048ac3d289878aa0297dbf6e8ebf5fbfc49c316983aa5f2"

  yield();
  Bot.sendMessage(Bot.received_msg.chat.id, "TransactionID:");
  //Bot.sendMessage(Bot.received_msg.chat.id, data_accept_0);

  char explorerlink[100];
  strcpy(explorerlink, "https://radians.nl/#/transaction/");
  strcat(explorerlink, data_accept_0);
  Bot.sendMessage(Bot.received_msg.chat.id, explorerlink);

}
