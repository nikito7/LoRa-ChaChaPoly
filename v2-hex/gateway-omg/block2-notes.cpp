// ZgatewayLORA


// void LORAtoMQTT() {
// ...
//    // Create packet and reserve null terminator space
// --- cut --- and delete ---

    // ### EASY HAN LORA ####

    byte packet[packetSize + 1];
    boolean binary = false;
    
    uint8_t polySize = CHA_CHA_POLY_TAG_SIZE +
                 CHA_CHA_POLY_IV_SIZE;

    if (packetSize > polySize & packetSize < 200)
    {
      // ### if packet size is Cha Cha ###

      byte rxArray[packetSize];

      for (int i = 0; i < packetSize; i++) {
        rxArray[i] = (char)LoRa.read();
      }

      // ### CHA_CHA_POLY ###

      byte tag[CHA_CHA_POLY_TAG_SIZE];
      byte plainText[packetSize - polySize];
      byte cipherText[packetSize - polySize];

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
        Serial.println("Decrypted!");
#       endif

        // hex to json

        LORAdata["byte0"] = plainText[0];
        LORAdata["fPort"] = plainText[1];

        LORAdata["id"] = plainText[2] << 24
                     | plainText[3] << 16
                     | plainText[4] << 8
                     | plainText[5];

        LORAdata["EBx"] = plainText[6];
        LORAdata["Ser"] = plainText[7];

        LORAdata["ERR"] = plainText[8]
                      | plainText[9];

        LORAdata["up"] = plainText[10] << 24
                     | plainText[11] << 16
                     | plainText[12] << 8
                     | plainText[13];

        LORAdata["HH"] = plainText[14];
        LORAdata["MM"] = plainText[15];
        LORAdata["SS"] = plainText[16];
      } // if verify
    } // if packet size is cha cha true
    // ### EASY HAN LORA EOF ####


// --- cut ---
//    LORAdata["rssi"] = (int)LoRa.packetRssi();

// EOF

