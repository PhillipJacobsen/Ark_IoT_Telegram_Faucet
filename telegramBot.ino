void telegramBotHandler() {
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    
  while (Bot.getUpdates())
  {
    u8g2.clearBuffer();
    u8g2.sendBuffer();

    telegram_PrintMsgDetails();
    u8g2.setFont(u8g2_font_unifont_t_symbols);
    u8g2.drawUTF8(0, 0, "New Telegram Msg");
    u8g2.sendBuffer();

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

      u8g2.drawUTF8(0, 15, "/start");
      u8g2.sendBuffer();

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
      u8g2.drawUTF8(0, 15, "/help");
      u8g2.sendBuffer();

      strcpy(TelegramMessage, "Hello ");
      strcat(TelegramMessage, Bot.received_msg.from.first_name);
      Bot.sendMessage(Bot.received_msg.chat.id, TelegramMessage);
      Bot.sendMessage(Bot.received_msg.chat.id, TEXT_HELP);
    }

    //--------------------------------------------
    // If /time command was received
    else if (strncmp(Bot.received_msg.text, "/time", strlen("/time")) == 0)
    {
      u8g2.drawUTF8(0, 15, "/time");
      u8g2.sendBuffer();

      time_t now = time(nullptr);   //  time_t is used to store the number of seconds since the epoch (normally 01/01/1970)
      Bot.sendMessage(Bot.received_msg.chat.id, ctime(&now));
      Serial.print("time is: ");
      Serial.println(ctime(&now));
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
      u8g2.drawUTF8(0, 15, "/address");
      u8g2.sendBuffer();

      strcpy(TelegramMessage, "Faucet Address: ");
      strcat(TelegramMessage, FaucetAddress);
      Bot.sendMessage(Bot.received_msg.chat.id, TelegramMessage);
    }

    //--------------------------------------------
    // If /balance command was received
    else if (strncmp(Bot.received_msg.text, "/balance", strlen("/balance")) == 0)
    {

      u8g2.drawUTF8(0, 15, "/balance");
      u8g2.sendBuffer();

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

        //see address.hpp for info on address verification
        if (Address::validate( Address(mybalanceAddress_char), BRIDGECHAIN_VERSION)) {
          Serial.println("Address Validated");
          //--------------------------------------------
          // Retrieve Wallet Nonce from blockchain before sending transaction
          getWallet_requested(mybalanceAddress_char);
          strcpy(TelegramMessage, "Your Wallet Balance: ");
          strcat(TelegramMessage, mywalletBalance);
          Bot.sendMessage(Bot.received_msg.chat.id, TelegramMessage);
        }
        else {
          Serial.println("not a valid address");
          Bot.sendMessage(Bot.received_msg.chat.id, "That was not a valid Radians address");
          break;
        }


      }
      else {
        Serial.println("not a valid address");
        Bot.sendMessage(Bot.received_msg.chat.id, "That was not a valid Radians address");
        break;
      }
    }


    //--------------------------------------------
    // If /request_ command was received
    else if (strncmp(Bot.received_msg.text, "/request_", strlen("/request_")) == 0)
    {
      u8g2.drawUTF8(0, 15, "/request_ADDRESS");
      u8g2.sendBuffer();

      String request = String(Bot.received_msg.text);
      if (request.charAt(9) == 'T')  {        //simple address verification
        String receiveaddress = request.substring(9, 9 + 34 + 1);       //Ark address is 34 digits long
        Serial.print("\nreceiveAddress: ");
        Serial.print(receiveaddress);
        receiveaddress.toCharArray(receiveaddress_char, 34 + 1);

        if (Address::validate( Address(receiveaddress_char), BRIDGECHAIN_VERSION)) {
          
          Serial.println("Address Validated");
        }
        else {
          u8g2.drawUTF8(0, 30, "Invalid ADDRESS");
          u8g2.sendBuffer();

          Serial.println("not a valid address");
          Bot.sendMessage(Bot.received_msg.chat.id, "That was not a valid Radians address");
          break;
        }

        //--------------------------------------------
        // Retrieve Wallet Nonce from blockchain before sending transaction
        getWallet();
        yield();
        //--------------------------------------------

        Serial.print("\nreceiveAddress char: ");
        Serial.print(receiveaddress_char);
        
          u8g2.drawUTF8(0, 30, "Sending Tokens..");
          u8g2.sendBuffer();
          
        //--------------------------------------------
        sendBridgechainTransaction();
      }

      else {
        Serial.println("not a valid address");
        Bot.sendMessage(Bot.received_msg.chat.id, "That was not a valid Radians address");
        break;
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
