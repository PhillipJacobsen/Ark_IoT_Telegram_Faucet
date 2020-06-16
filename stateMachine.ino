/********************************************************************************
  This file contains the main Finite State Machine logic

********************************************************************************/

//--------------------------------------------
// Mealy Finite State Machine
// The state machine logic is executed once each cycle of the Arduino "main" loop.
//--------------------------------------------
void StateMachine() {
  switch (state) {

    //--------------------------------------------
    // State 0
    // Initial state after microcontroller powers up and initializes the various peripherals
    // Transitions to State 1 once WiFi is connected
    case STATE_0: {
        if (WiFiMQTTclient.isWifiConnected()) {          //wait for WiFi to connect
          state = STATE_1;

          Serial.print("\nState: ");
          Serial.println(state);
          Serial.println("WiFi Connected");
        }
        else {
          state = STATE_0;
        }
        break;
      }

    //--------------------------------------------
    // State 1
    // Transitions to state 2 once connected to MQTT broker
    // Return to state 0 if WiFi disconnects
    case STATE_1: {
        if (!WiFiMQTTclient.isWifiConnected()) {     //check for WiFi disconnect
          state = STATE_0;
          Serial.print("\nState: ");
          Serial.println(state);
          Serial.println("WiFi Disconnected");
        }
        else if (WiFiMQTTclient.isMqttConnected()) {  //wait for MQTT connect
          state = STATE_2;
          Serial.print("\nState: ");
          Serial.println(state);
          Serial.println("MQTT Connected");
          
        }
        else {
          state = STATE_1;
        }
        break;
      }

    //--------------------------------------------
    // State 2
    // Transistions to state 3 once TBD..........
    // Return to state 0 if WiFi disconnects
    // Returns to state 1 if MQTT disconnects
    case STATE_2: {
        if (!WiFiMQTTclient.isWifiConnected()) {     //check for WiFi disconnect
          state = STATE_0;
          Serial.print("\nState: ");
          Serial.println(state);
          Serial.println("WiFi Disconnected");
        }
        else if (!WiFiMQTTclient.isMqttConnected()) {  //check for MQTT disconnect
          state = STATE_1;
          Serial.print("\nState: ");
          Serial.println(state);
          Serial.println("MQTT Disconnected");
        }
        //        else if (ARK_status) {  //wait for ARK network connect
        //          state = STATE_3;
        //          Serial.print("State: ");
        //          Serial.println(state);
        //        }

        else {
          state = STATE_2;
        }
        break;
      }

  }
}
