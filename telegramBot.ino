  void telegramBotHandler() {

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
      Bot.sendMessage(Bot.received_msg.chat.id, FaucetAddress);
    }

    //--------------------------------------------
    // If /balance command was received
    else if (strncmp(Bot.received_msg.text, "/balance", strlen("/balance")) == 0)
    {
      //--------------------------------------------
      //  Retrieve Wallet Nonce and Balance
      getWallet();
      Bot.sendMessage(Bot.received_msg.chat.id, "faucet balance:");
      Bot.sendMessage(Bot.received_msg.chat.id, bridgechainWallet.walletBalance);
    }


    //--------------------------------------------
    // If /mybalance_ command was received
    //--------------------------------------------
    else if (strncmp(Bot.received_msg.text, "/mybalance_", strlen("/mybalance_")) == 0)
    {
      String mybalance = String(Bot.received_msg.text);
      if (mybalance.charAt(11) == 'T')  {        //simple address verification
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
        Bot.sendMessage(Bot.received_msg.chat.id, "Your balance is:");
        Bot.sendMessage(Bot.received_msg.chat.id, mywalletBalance);
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

        yield();

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
