void telegramBotHandler() {

  while (Bot.getUpdates())
  {


    telegram_PrintMsgDetails();
    // Show received message text
    //Serial.println("");
    //Serial.println("Received message:");
    //Serial.println(Bot.received_msg.text);
    //Serial.println("");

    char TelegramMessage[100] = {0};
    //--------------------------------------------
    // If /start command was received
    if (strncmp(Bot.received_msg.text, "/start", strlen("/start")) == 0)
    {

      //Serial.printf("  From user name: %s %s\n", Bot.received_msg.from.first_name,Bot.received_msg.from.last_name);

      //sprintf(TelegramMessage, "Welcome %02X%02X%02X%02X%02X%02X", "Welcome ", Bot.received_msg.from.first_name);  // Example: B4E62DA8EF6D
      // http://www.cplusplus.com/reference/cstdio/printf/

      // Send a Telegram message for start
      Bot.sendMessage(Bot.received_msg.chat.id, TEXT_START);
    }


    //--------------------------------------------
    // If /help command was received
    else if (strncmp(Bot.received_msg.text, "/help", strlen("/help")) == 0)
    {
      strcpy(TelegramMessage, "Hello ");
      strcat(TelegramMessage, Bot.received_msg.from.first_name);
      Bot.sendMessage(Bot.received_msg.chat.id, TelegramMessage);
      Bot.sendMessage(Bot.received_msg.chat.id, TEXT_HELP);
    }


    //--------------------------------------------
    // If /ledon command was received
    else if (strncmp(Bot.received_msg.text, "/ledon", strlen("/ledon")) == 0)
    {
      // Turn on LED
      led_status = 1;
      digitalWrite(LED_PIN, HIGH);

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
      digitalWrite(LED_PIN, LOW);

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
      strcpy(TelegramMessage, "Faucet Address: ");
      strcat(TelegramMessage, FaucetAddress);
      Bot.sendMessage(Bot.received_msg.chat.id, TelegramMessage);
    }

    //--------------------------------------------
    // If /balance command was received
    else if (strncmp(Bot.received_msg.text, "/balance", strlen("/balance")) == 0)
    {
      //--------------------------------------------
      //  Retrieve Wallet Nonce and Balance
      getWallet();
      strcpy(TelegramMessage, "Faucet Balance: ");
      strcat(TelegramMessage, bridgechainWallet.walletBalance);
      Bot.sendMessage(Bot.received_msg.chat.id, TelegramMessage);
    }


    //--------------------------------------------
    // If /mybalance_ command was received
    //--------------------------------------------
    else if (strncmp(Bot.received_msg.text, "/mybalance_", strlen("/mybalance_")) == 0)
    {
      String mybalance = String(Bot.received_msg.text);
      if (mybalance.charAt(11) == 'T')  {                                     //simple address verification
        String mybalanceAddress = mybalance.substring(11, 11 + 34 + 1);       //Ark address is 34 digits long
        Serial.print("\naddress: ");
        Serial.print(mybalanceAddress);

        //--------------------------------------------
        mybalanceAddress.toCharArray(mybalanceAddress_char, 34 + 1);
        Serial.print("\nreceiveAddress char: ");
        Serial.print(mybalanceAddress_char);

        //--------------------------------------------
        // Retrieve Wallet Nonce from blockchain before sending transaction
        getWallet_requested(mybalanceAddress_char);
        strcpy(TelegramMessage, "Your Wallet Balance: ");
        strcat(TelegramMessage, mywalletBalance);
        Bot.sendMessage(Bot.received_msg.chat.id, TelegramMessage);
      }
      else {
        break;
      }
    }


    //--------------------------------------------
    // If /request_ command was received
    else if (strncmp(Bot.received_msg.text, "/request_", strlen("/request_")) == 0)
    {
      String request = String(Bot.received_msg.text);
      if (request.charAt(9) == 'T')  {        //simple address verification
        String receiveaddress = request.substring(9, 9 + 34 + 1);       //Ark address is 34 digits long
        Serial.print("\nreceiveAddress: ");
        Serial.print(receiveaddress);

        //--------------------------------------------
        // Retrieve Wallet Nonce from blockchain before sending transaction
        getWallet();

        yield();

        //--------------------------------------------
        receiveaddress.toCharArray(receiveaddress_char, 34 + 1);
        Serial.print("\nreceiveAddress char: ");
        Serial.print(receiveaddress_char);

        //--------------------------------------------
        sendBridgechainTransaction();
      }

      else {
        break;
      }

    }
  }

  // Feed the Watchdog
  yield();
}



void telegram_PrintMsgDetails() {
  Serial.println("\n-----------------------------------------");
  Serial.println("Received message.");

  Serial.printf("  From chat ID: %s\n", Bot.received_msg.chat.id);
  Serial.printf("  From chat type: %s\n", Bot.received_msg.chat.type);
  Serial.printf("  From chat alias: %s\n", Bot.received_msg.chat.username);
  Serial.printf("  From chat name: %s %s\n", Bot.received_msg.chat.first_name,
                Bot.received_msg.chat.last_name);
  Serial.printf("  From chat title: %s\n", Bot.received_msg.chat.title);
  if (Bot.received_msg.chat.all_members_are_administrators)
    Serial.println("  From chat where all members are admins.");
  else
    Serial.println("  From chat where not all members are admins.");

  Serial.printf("  From user ID: %s\n", Bot.received_msg.from.id);
  Serial.printf("  From user alias: %s\n", Bot.received_msg.from.username);
  Serial.printf("  From user name: %s %s\n", Bot.received_msg.from.first_name,
                Bot.received_msg.from.last_name);
  Serial.printf("  From user with language code: %s\n", Bot.received_msg.from.language_code);
  if (Bot.received_msg.from.is_bot)
    Serial.println("  From user that is a Bot.");
  else
    Serial.println("  From user that is not a Bot.");

  Serial.printf("  Message ID: %d\n", Bot.received_msg.message_id);
  Serial.printf("  Message sent date (UNIX epoch time): %ul\n", Bot.received_msg.date);
  Serial.printf("  Text: %s\n", Bot.received_msg.text);
  Serial.printf("-----------------------------------------\n");
}
