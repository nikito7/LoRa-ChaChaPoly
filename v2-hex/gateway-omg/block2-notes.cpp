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
        for (uint8_t i = 0; i < sizeof(plainText); i++)
        {
          plainText[i] = 0x78;
        }

        // decrypt message from cipherText to plainText
        // output is valid only if result is true
        bool verify = ChaChaPolyCipher.decrypt(key, iv, auth, cipherText, plainText, tag);

        if (verify)
        {
          // hex to json

          //LORAdata["byte0"] = plainText[0];
          //LORAdata["byte1"] = plainText[1];


          LORAdata["id"] = std::to_string(plainText[2] << 24
                         | plainText[3] << 16
                         | plainText[4] << 8
                         | plainText[5]) + "_EB" +
                         std::to_string(plainText[6]);

          //LORAdata["EBx"] = plainText[6];
          LORAdata["Ser"] = plainText[7];

          LORAdata["ERR"] = plainText[8] << 8
                          | plainText[9];

          LORAdata["up"] = plainText[10] << 24
                         | plainText[11] << 16
                         | plainText[12] << 8
                         | plainText[13];

          //LORAdata["HH"] = plainText[14];
          //LORAdata["MM"] = plainText[15];
          //LORAdata["SS"] = plainText[16];

          LORAdata["Clock"] = std::to_string(plainText[14])
                      + ":" + std::to_string(plainText[15])
                      + ":" + std::to_string(plainText[16]);

          LORAdata["VL1"] = plainText[17] << 8
                          | plainText[18];
          LORAdata["CL1"] = plainText[19] << 8
                          | plainText[20];
          LORAdata["VL2"] = plainText[21] << 8
                          | plainText[22];
          LORAdata["CL2"] = plainText[23] << 8
                          | plainText[24];
          LORAdata["VL3"] = plainText[25] << 8
                          | plainText[26];
          LORAdata["CL3"] = plainText[27] << 8
                          | plainText[28];
          LORAdata["CLT"] = plainText[29] << 8
                          | plainText[30];

          LORAdata["FR"] = plainText[31] << 8
                          | plainText[32];

          LORAdata["PF"] = plainText[33] << 8
                          | plainText[34];
          LORAdata["PF1"] = plainText[35] << 8
                          | plainText[36];
          LORAdata["PF2"] = plainText[37] << 8
                          | plainText[38];
          LORAdata["PF3"] = plainText[39] << 8
                          | plainText[40];

          LORAdata["API"] = plainText[41] << 24
                         | plainText[42] << 16
                         | plainText[43] << 8
                         | plainText[44];

          LORAdata["APE"] = plainText[45] << 24
                         | plainText[46] << 16
                         | plainText[47] << 8
                         | plainText[48];

          // 32bits to 16bits
          LORAdata["API1"] = plainText[49] << 8
                          | plainText[50];
          LORAdata["APE1"] = plainText[51] << 8
                          | plainText[52];
          LORAdata["API2"] = plainText[53] << 8
                          | plainText[54];
          LORAdata["APE2"] = plainText[55] << 8
                          | plainText[56];
          LORAdata["API3"] = plainText[57] << 8
                          | plainText[58];
          LORAdata["APE3"] = plainText[59] << 8
                          | plainText[60];
          // back to normal size

          // Total Energy Tarifas Import kWh
          LORAdata["TET1"] = plainText[61] << 24
                         | plainText[62] << 16
                         | plainText[63] << 8
                         | plainText[64];

          LORAdata["TET2"] = plainText[65] << 24
                         | plainText[66] << 16
                         | plainText[67] << 8
                         | plainText[68];

          LORAdata["TET3"] = plainText[69] << 24
                         | plainText[70] << 16
                         | plainText[71] << 8
                         | plainText[72];

          // Total Energy kWh
          LORAdata["TEI"] = plainText[73] << 24
                         | plainText[74] << 16
                         | plainText[75] << 8
                         | plainText[76];

          LORAdata["TEE"] = plainText[77] << 24
                         | plainText[78] << 16
                         | plainText[79] << 8
                         | plainText[80];

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

