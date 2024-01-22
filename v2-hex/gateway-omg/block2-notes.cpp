// ZgatewayLORA.ino


// void LORAtoMQTT() {
// ...
//    if (deviceId == WIPHONE) {
//      _WiPhoneToMQTT(packet, LORAdata);
//    } else if (binary) {

// --- cut ---

      // ###
      // ### EASY HAN LORA ####

      bool noVerify = false;
      uint8_t polySize = CHA_CHA_POLY_TAG_SIZE +
                 CHA_CHA_POLY_IV_SIZE;

      if (packetSize > polySize & packetSize < 250)
      {
        // ### if packet size is Cha Cha ###

        byte rxArray[packetSize];
        memcpy(rxArray, packet, packetSize);

        // ### CHA_CHA_POLY ###

        byte tag[CHA_CHA_POLY_TAG_SIZE];
        byte plainText[sizeof(rxArray) - polySize];
        byte cipherText[sizeof(plainText)];

        // copy arrays
        memcpy(cipherText, rxArray, sizeof(cipherText));
        memcpy(tag, &rxArray[sizeof(cipherText)], sizeof(tag));
        memcpy(iv, &rxArray[sizeof(cipherText) +
                    sizeof(tag)], sizeof(iv));

        // clear array
        for (int i = 0; i < sizeof(plainText); i++)
        {
          plainText[i] = 0x77;
        }

        // decrypt message from cipherText to plainText
        // output is valid only if result is true
        bool verify = ChaChaPolyCipher.decrypt(key, iv, auth, cipherText, plainText, tag);

        if (verify)
        {
          // hex to json

          LORAdata["byte0"] = plainText[0];
          LORAdata["byte1"] = plainText[1];

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

        } // eof if verify
        else
        {
          noVerify = true;
        } // eof else if verify
      } // eof if packet size is cha cha true
      else
      {
        noVerify = true;
      }

      if (noVerify)
      {
        LORAdata["id"] = "NoVerify";

        // --- original omg code ---

        if (LORAConfig.onlyKnown) {
          Log.trace(F("Ignoring non identifiable packet" CR));
           return;
        }
        // We have non-ascii data: create hex string of the data
        char hex[packetSize * 2 + 1];
        _rawToHex(packet, hex, packetSize);
        // Terminate with a null character
        hex[packetSize * 2] = 0;

        LORAdata["hex"] = hex;

        // --- original omg code ---
      } // if noVerify
      // ### EASY HAN LORA EOF ####
      // ###

// --- cut ---

//    } else {
//      // ascii payload

// EOF

