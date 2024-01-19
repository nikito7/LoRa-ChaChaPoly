// ZgatewayLORA


// void LORAtoMQTT() {
// ...
//    // Create packet and reserve null terminator space
// --- cut --- and adapt ---

    // ### EASY HAN LORA ####

    byte packet[packetSize + 1];
    boolean binary = false;

    if (packetSize == CHA_CHA_POLY_MESSAGE_SIZE + 
                 CHA_CHA_POLY_TAG_SIZE +
                 CHA_CHA_POLY_IV_SIZE)
    {
      // ### if packet size is Cha Cha ###

      byte rxArray[CHA_CHA_POLY_MESSAGE_SIZE + 
                   CHA_CHA_POLY_TAG_SIZE +
                   CHA_CHA_POLY_IV_SIZE];

      for (int i = 0; i < packetSize; i++) {
        rxArray[i] = (char)LoRa.read();
      }

      // ### CHA_CHA_POLY ###

      byte tag[CHA_CHA_POLY_TAG_SIZE];
      byte plainText[CHA_CHA_POLY_MESSAGE_SIZE];
      byte cipherText[CHA_CHA_POLY_MESSAGE_SIZE];

      memcpy(cipherText, rxArray, sizeof(cipherText));
      memcpy(tag, &rxArray[sizeof(cipherText)], sizeof(tag));
      memcpy(iv, &rxArray[sizeof(cipherText) +
                    sizeof(tag)], sizeof(iv));

#     ifdef CHA_CHA_DEBUG
      Serial.println(" ");
      Serial.println("rxArray:");  

      for(i=0; i<sizeof(rxArray); i++)
      {
        printHex(rxArray[i]);
      }

      Serial.println(" ");
      Serial.println("cipherText:");  

      for(i=0; i<sizeof(cipherText); i++)
      {
        printHex(cipherText[i]);
      }

      Serial.println(" ");
      Serial.println("Tag:");

      for(i=0; i<sizeof(tag); i++)
      {
        printHex(tag[i]);
      } 

      Serial.println(" ");
      Serial.println("IV:");

      for(i=0; i<sizeof(iv); i++)
      {
        printHex(iv[i]);
      } 
#     endif

      // decrypt message from cipherText to plainText
      // output is valid only if result is true
      bool verify = ChaChaPolyCipher.decrypt(key, iv, auth, cipherText, plainText, tag);

      if (verify)
      {
#       ifdef CHA_CHA_DEBUG
        Serial.println(" ");
        Serial.println("Decrypted:");  
        String decrypted = String((char*)plainText);
        Serial.println(decrypted);
#       endif

        for (int i = 0; i < sizeof(plainText); i++) {
          packet[i] = plainText[i];
        }
      }
      else
      {
        binary = true;
      }
      // ### CHA_CHA_POLY EOF ###
    // ### end if packet size is cha cha true ###
    }
    else
    {
      for (int i = 0; i < packetSize; i++) {
        packet[i] = (char)LoRa.read();

        if (packet[i] < 32 || packet[i] > 127)
          binary = true;
      }
    }
    // ### EASY HAN LORA EOF ####


// --- cut ---
// // Terminate with a null character in case we have a string

// EOF

