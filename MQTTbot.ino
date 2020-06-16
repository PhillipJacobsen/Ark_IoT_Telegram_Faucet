//
//
////String request = String(Bot.received_msg.text);
//String request = String(payload);
//
//String receiveaddress = request;
//Serial.print("\nreceiveAddress: ");
//Serial.print(receiveaddress);
//receiveaddress.toCharArray(receiveaddress_char, 34 + 1);
//
//if (Address::validate( Address(receiveaddress_char), BRIDGECHAIN_VERSION)) {
//  Serial.println("Address Validated");
//}
//else {
//  Serial.println("not a valid address");
//  Bot.sendMessage(Bot.received_msg.chat.id, "That was not a valid Radians address");
//  break;
//}
//
////--------------------------------------------
//// Retrieve Wallet Nonce from blockchain before sending transaction
//getWallet();
//yield();
////--------------------------------------------
//
//Serial.print("\nreceiveAddress char: ");
//Serial.print(receiveaddress_char);
//
////--------------------------------------------
//sendBridgechainTransaction();
//}
//
//
//}
