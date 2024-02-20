// LoRa Modbus Node

#include <HardwareSerial.h>
#include <LoRa.h>
#include <ModbusMaster.h>

#ifdef ESP8266
#define LORA_SCK 14
#define LORA_MISO 12
#define LORA_MOSI 13
#define LORA_SS 15
#define LORA_RST -1
#define LORA_DI0 5
#endif

#ifdef ESP32C3
#define LORA_SCK 4
#define LORA_MISO 5
#define LORA_MOSI 6
#define LORA_SS 7
#define LORA_RST 2
#define LORA_DI0 10
#endif

// ### CHA_CHA_POLY ###

#include "../../easyhan_secrets_cha.h"
#include "ChaChaPolyHelper.h"

byte iv[CHA_CHA_POLY_IV_SIZE];

// ### CHA_CHA_POLY EOF ###

#include "han.h"

HardwareSerial &HwSerial = Serial;
ModbusMaster node;

void hanBlink() {
#ifdef ESP8266
  digitalWrite(2, LOW);
  delay(50);
  digitalWrite(2, HIGH);
#endif
}

void setup() {
  delay(2000);

#ifdef ESP8266
  pinMode(2, OUTPUT);
#endif

  delay(1000);

  HwSerial.begin(9600, SERIAL_8N1);

  while (!HwSerial)
    ;

  SPI.begin();
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  LoRa.enableCrc();
  LoRa.setSyncWord(0x12);

  if (!LoRa.begin(868E6)) {
    while (1)
      ;
  }

  hanRead = millis() + 31000;
  loraRead = millis() + 15000;

}  // end setup

void setDelayError(uint8_t hanRes) {
  hanCode = hanRes;
  if (hanRes == 0xe2) {
    hanDelay = hanDelayError;
    hanIndex = 0;
  } else {
    hanDelay = hanDelayWait;
  }
}

void loop() {
  //

  uint32_t up = millis() / 1000;

  if (hanRead + hanDelay < millis()) {
    hanWork = true;
  }

  if (loraRead + loraDelay < millis()) {
    loraWork = true;
  }

  // # # # # # # # # # #
  // EASYHAN MODBUS BEGIN
  // # # # # # # # # # #

  uint8_t hRes;

  // # # # # # # # # # #
  // Setup
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 0)) {
    //
    HwSerial.end();

    delay(100);

    HwSerial.begin(9600, SERIAL_8N1);

    delay(100);

    node.begin(1, HwSerial);

    delay(100);

    uint8_t testserial;

    testserial = node.readInputRegisters(0x0001, 1);
    if (testserial == node.ku8MBSuccess) {
      hanCFG = 1;
      hanIndex++;
    } else {
      hanCode = testserial;
      hanERR++;
      setDelayError(testserial);
      //
      HwSerial.end();
      delay(100);
      HwSerial.begin(9600, SERIAL_8N2);
      delay(100);
      //
      testserial = node.readInputRegisters(0x0001, 1);
      if (testserial == node.ku8MBSuccess) {
        hanCFG = 2;
        hanIndex++;
      }
      //
    }

    delay(100);

    // Detect EB Type

    uint8_t testEB;
    uint16_t hanDTT = 0;

    testEB = node.readInputRegisters(0x0070, 2);
    if (testEB == node.ku8MBSuccess) {
      //
      hanDTT = node.getResponseBuffer(0);
      if (hanDTT > 0) {
        hanEB = 3;
      } else {
        hanEB = 1;
      }
      //
    } else {
      hanERR++;
      hanCode = testEB;
      setDelayError(testEB);
      hanEB = 1;
    }
    hanRead = millis();
    hanWork = false;
  }

  // # # # # # # # # # #
  // Clock ( 12 bytes )
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 1)) {
    hRes = node.readInputRegisters(0x0001, 1);
    if (hRes == node.ku8MBSuccess) {
      hanYY = node.getResponseBuffer(0);
      hanMT = node.getResponseBuffer(1) >> 8;
      hanDD = node.getResponseBuffer(1) & 0xFF;
      hanWD = node.getResponseBuffer(2) >> 8;
      hanHH = node.getResponseBuffer(2) & 0xFF;
      hanMM = node.getResponseBuffer(3) >> 8;
      hanSS = node.getResponseBuffer(3) & 0xFF;
      hanBlink();
      hanDelay = hanDelayWait;
    } else {
      hanERR++;
      setDelayError(hRes);
    }
    hanRead = millis();
    hanWork = false;
    hanIndex++;
  }

  // # # # # # # # # # #
  // Voltage Current
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 2)) {
    if (hanEB == 3) {
      hRes = node.readInputRegisters(0x006c, 7);
      if (hRes == node.ku8MBSuccess) {
        hanVL1 = node.getResponseBuffer(0);
        hanCL1 = node.getResponseBuffer(1);
        hanVL2 = node.getResponseBuffer(2);
        hanCL2 = node.getResponseBuffer(3);
        hanVL3 = node.getResponseBuffer(4);
        hanCL3 = node.getResponseBuffer(5);
        hanCLT = node.getResponseBuffer(6);
        hanBlink();
        hanDelay = hanDelayWait;
      } else {
        hanERR++;
        setDelayError(hRes);
      }
    } else {
      hRes = node.readInputRegisters(0x006c, 2);
      if (hRes == node.ku8MBSuccess) {
        hanVL1 = node.getResponseBuffer(0);
        hanCL1 = node.getResponseBuffer(1);
        hanBlink();
        hanDelay = hanDelayWait;
      } else {
        hanERR++;
        setDelayError(hRes);
      }
    }
    hanRead = millis();
    hanWork = false;
    hanIndex++;
  }

  // # # # # # # # # # #
  // Active Power Import/Export 73 (tri)
  // Power Factor (mono) (79..)
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 3)) {
    if (hanEB == 3) {
      hRes = node.readInputRegisters(0x0073, 8);
      if (hRes == node.ku8MBSuccess) {
        hanAPI1 = node.getResponseBuffer(1) |
                  node.getResponseBuffer(0) << 16;
        hanAPE1 = node.getResponseBuffer(3) |
                  node.getResponseBuffer(2) << 16;
        hanAPI2 = node.getResponseBuffer(5) |
                  node.getResponseBuffer(4) << 16;
        hanAPE2 = node.getResponseBuffer(7) |
                  node.getResponseBuffer(6) << 16;
        hanAPI3 = node.getResponseBuffer(9) |
                  node.getResponseBuffer(8) << 16;
        hanAPE3 = node.getResponseBuffer(11) |
                  node.getResponseBuffer(10) << 16;
        hanAPI = node.getResponseBuffer(13) |
                 node.getResponseBuffer(12) << 16;
        hanAPE = node.getResponseBuffer(15) |
                 node.getResponseBuffer(14) << 16;
        hanBlink();
        hanDelay = hanDelayWait;
      } else {
        hanERR++;
        setDelayError(hRes);
      }
    } else {
      hRes = node.readInputRegisters(0x0079, 3);
      if (hRes == node.ku8MBSuccess) {
        hanAPI = node.getResponseBuffer(1) |
                 node.getResponseBuffer(0) << 16;
        hanAPE = node.getResponseBuffer(3) |
                 node.getResponseBuffer(2) << 16;
        hanPF = node.getResponseBuffer(4);
        hanBlink();
        hanDelay = hanDelayWait;
      } else {
        hanERR++;
        setDelayError(hRes);
      }
    }
    hanRead = millis();
    hanWork = false;
    hanIndex++;
  }

  // # # # # # # # # # #
  // Power Factor (7B) / Frequency (7F)
  // Power Factor (tri)
  // Frequency (mono)
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 4)) {
    if (hanEB == 3) {
      hRes = node.readInputRegisters(0x007b, 5);
      if (hRes == node.ku8MBSuccess) {
        hanPF = node.getResponseBuffer(0);
        hanPF1 = node.getResponseBuffer(1);
        hanPF2 = node.getResponseBuffer(2);
        hanPF3 = node.getResponseBuffer(3);
        hanFreq = node.getResponseBuffer(4);
        hanBlink();
        hanDelay = hanDelayWait;
      } else {
        hanERR++;
        setDelayError(hRes);
      }
    } else {
      hRes = node.readInputRegisters(0x007f, 1);
      if (hRes == node.ku8MBSuccess) {
        hanFreq = node.getResponseBuffer(0);
        hanBlink();
        hanDelay = hanDelayWait;
      } else {
        hanERR++;
        setDelayError(hRes);
      }
    }
    hanRead = millis();
    hanWork = false;
    hanIndex++;
  }

  // # # # # # # # # # #
  // Total Energy Tarifas (kWh) 26
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 5)) {
    hRes = node.readInputRegisters(0x0026, 3);
    if (hRes == node.ku8MBSuccess) {
      hanTET1 = node.getResponseBuffer(1) |
                node.getResponseBuffer(0) << 16;
      hanTET2 = node.getResponseBuffer(3) |
                node.getResponseBuffer(2) << 16;
      hanTET3 = node.getResponseBuffer(5) |
                node.getResponseBuffer(4) << 16;
      hanBlink();
      hanDelay = hanDelayWait;
    } else {
      hanERR++;
      setDelayError(hRes);
    }
    hanRead = millis();
    hanWork = false;
    hanIndex++;
  }

  // # # # # # # # # # #
  // Total Energy (total) (kWh) 16
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 6)) {
    hRes = node.readInputRegisters(0x0016, 2);
    if (hRes == node.ku8MBSuccess) {
      hanTEI = node.getResponseBuffer(1) |
               node.getResponseBuffer(0) << 16;
      hanTEE = node.getResponseBuffer(3) |
               node.getResponseBuffer(2) << 16;
      hanBlink();
      hanDelay = hanDelayWait;
    } else {
      hanERR++;
      setDelayError(hRes);
    }
    hanRead = millis();
    hanWork = false;
    hanIndex++;
  }

  // # # # # # # # # # #
  // Load Profile
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 7)) {
    hRes = node.readLastProfile(0x06, 0x01);
    if (hRes == node.ku8MBSuccess) {
      hLP1YY = node.getResponseBuffer(0);
      hLP1MT = node.getResponseBuffer(1) >> 8;
      hLP1DD = node.getResponseBuffer(1) & 0xFF;
      // hLP1WD = node.getResponseBuffer(2) >> 8;
      hLP1HH = node.getResponseBuffer(2) & 0xFF;
      hLP1MM = node.getResponseBuffer(3) >> 8;
      // hLP1SS = node.getResponseBuffer(3) & 0xFF;

      // tweaked to 16bits. branch: LP1.
      hLP2 = node.getResponseBuffer(6);

      hLP3 = node.getResponseBuffer(8) |
             node.getResponseBuffer(7) << 16;
      hLP4 = node.getResponseBuffer(10) |
             node.getResponseBuffer(9) << 16;
      hLP5 = node.getResponseBuffer(12) |
             node.getResponseBuffer(11) << 16;
      hLP6 = node.getResponseBuffer(14) |
             node.getResponseBuffer(13) << 16;

      hanBlink();
      hanDelay = hanDelayWait;
    } else {
      hanERR++;
      setDelayError(hRes);
    }
    hanRead = millis();
    hanWork = false;
    hanIndex++;
  }

  // # # # # # # # # # #
  // Contract
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 8)) {
    hRes = node.readInputRegisters(0x000C, 1);
    if (hRes == node.ku8MBSuccess) {
      hCT1 = (node.getResponseBuffer(1) |
              node.getResponseBuffer(0) << 16) /
             1000.0;
      hanBlink();
      hanDelay = hanDelayWait;
    } else {
      hanERR++;
      setDelayError(hRes);
    }
    hanRead = millis();
    hanWork = false;
    hanIndex++;
  }

  // # # # # # # # # # #
  // Tariff
  // # # # # # # # # # #

  if (hanWork & (hanIndex == 9)) {
    hRes = node.readInputRegisters(0x000B, 1);
    if (hRes == node.ku8MBSuccess) {
      hTariff = node.getResponseBuffer(0) >> 8;
      hanBlink();
      hanDelay = hanDelayWait;
    } else {
      hanERR++;
      setDelayError(hRes);
    }
    hanRead = millis();
    hanWork = false;
    hanIndex++;
  }

  // # # # # # # # # # #
  // EASYHAN MODBUS EOF
  // # # # # # # # # # #

  if (hanERR > 900) {
    hanERR = 0;
  }

  if (hanIndex > 9) {
    hanIndex = 1;
  }

  // # # # # # # # # # #
  // hex data to send
  // # # # # # # # # # #

  if (loraWork) {
    //

    ChaChaPolyCipher.generateIv(iv);

    byte plainText[CHA_CHA_POLY_MESSAGE_SIZE];

    // clear array
    for (uint8_t i = 0; i < sizeof(plainText); i++) {
      plainText[i] = 0xfe;
    }

    // data

#ifdef ESP8266
    uint32_t devID = ESP.getChipId();
#endif

#ifdef ESP32C3
    uint32_t devID = 777777;
#endif

    if (loraIndex > 1) {
      //
      uint32_t fPort32 = 7001;

      plainText[0] = fPort32 >> 8;
      plainText[1] = fPort32 & 0xFF;
      //
      plainText[2] = (devID & 0xFF000000) >> 24;
      plainText[3] = (devID & 0x00FF0000) >> 16;
      plainText[4] = (devID & 0x0000FF00) >> 8;
      plainText[5] = (devID & 0X000000FF);
      //
      plainText[6] = hanEB;
      plainText[7] = hanCFG;
      //
      plainText[8] = hanERR >> 8;
      plainText[9] = hanERR & 0xFF;
      //
      plainText[10] = (up & 0xFF000000) >> 24;
      plainText[11] = (up & 0x00FF0000) >> 16;
      plainText[12] = (up & 0x0000FF00) >> 8;
      plainText[13] = (up & 0X000000FF);
      //
      plainText[14] = hanHH;
      plainText[15] = hanMM;
      plainText[16] = hanSS;
      //
      plainText[17] = hanVL1 >> 8;
      plainText[18] = hanVL1 & 0xFF;
      plainText[19] = hanCL1 >> 8;
      plainText[20] = hanCL1 & 0xFF;
      plainText[21] = hanVL2 >> 8;
      plainText[22] = hanVL2 & 0xFF;
      plainText[23] = hanCL2 >> 8;
      plainText[24] = hanCL2 & 0xFF;
      plainText[25] = hanVL3 >> 8;
      plainText[26] = hanVL3 & 0xFF;
      plainText[27] = hanCL3 >> 8;
      plainText[28] = hanCL3 & 0xFF;
      plainText[29] = hanCLT >> 8;
      plainText[30] = hanCLT & 0xFF;
      //
      plainText[31] = hanFreq >> 8;
      plainText[32] = hanFreq & 0xFF;
      //
      plainText[33] = hanPF >> 8;
      plainText[34] = hanPF & 0xFF;
      plainText[35] = hanPF1 >> 8;
      plainText[36] = hanPF1 & 0xFF;
      plainText[37] = hanPF2 >> 8;
      plainText[38] = hanPF2 & 0xFF;
      plainText[39] = hanPF3 >> 8;
      plainText[40] = hanPF3 & 0xFF;
      //
      plainText[41] = (hanAPI & 0xFF000000) >> 24;
      plainText[42] = (hanAPI & 0x00FF0000) >> 16;
      plainText[43] = (hanAPI & 0x0000FF00) >> 8;
      plainText[44] = (hanAPI & 0X000000FF);
      //
      plainText[45] = (hanAPE & 0xFF000000) >> 24;
      plainText[46] = (hanAPE & 0x00FF0000) >> 16;
      plainText[47] = (hanAPE & 0x0000FF00) >> 8;
      plainText[48] = (hanAPE & 0X000000FF);
      // 32bits to 16bits
      plainText[49] = (hanAPI1 & 0x0000FF00) >> 8;
      plainText[50] = (hanAPI1 & 0X000000FF);
      plainText[51] = (hanAPE1 & 0x0000FF00) >> 8;
      plainText[52] = (hanAPE1 & 0X000000FF);
      //
      plainText[53] = (hanAPI2 & 0x0000FF00) >> 8;
      plainText[54] = (hanAPI2 & 0X000000FF);
      plainText[55] = (hanAPE2 & 0x0000FF00) >> 8;
      plainText[56] = (hanAPE2 & 0X000000FF);
      //
      plainText[57] = (hanAPI3 & 0x0000FF00) >> 8;
      plainText[58] = (hanAPI3 & 0X000000FF);
      plainText[59] = (hanAPE3 & 0x0000FF00) >> 8;
      plainText[60] = (hanAPE3 & 0X000000FF);
      //
      plainText[61] = (hanTET1 & 0xFF000000) >> 24;
      plainText[62] = (hanTET1 & 0x00FF0000) >> 16;
      plainText[63] = (hanTET1 & 0x0000FF00) >> 8;
      plainText[64] = (hanTET1 & 0X000000FF);
      //
      plainText[65] = (hanTET2 & 0xFF000000) >> 24;
      plainText[66] = (hanTET2 & 0x00FF0000) >> 16;
      plainText[67] = (hanTET2 & 0x0000FF00) >> 8;
      plainText[68] = (hanTET2 & 0X000000FF);
      //
      plainText[69] = (hanTET3 & 0xFF000000) >> 24;
      plainText[70] = (hanTET3 & 0x00FF0000) >> 16;
      plainText[71] = (hanTET3 & 0x0000FF00) >> 8;
      plainText[72] = (hanTET3 & 0X000000FF);
      //
      plainText[73] = (hanTEI & 0xFF000000) >> 24;
      plainText[74] = (hanTEI & 0x00FF0000) >> 16;
      plainText[75] = (hanTEI & 0x0000FF00) >> 8;
      plainText[76] = (hanTEI & 0X000000FF);
      //
      plainText[77] = (hanTEE & 0xFF000000) >> 24;
      plainText[78] = (hanTEE & 0x00FF0000) >> 16;
      plainText[79] = (hanTEE & 0x0000FF00) >> 8;
      plainText[80] = (hanTEE & 0X000000FF);
      //
      plainText[81] = hanCode;
      // LP test
      plainText[82] = hLP1YY >> 8;
      plainText[83] = hLP1YY & 0xFF;
      plainText[84] = hLP1MT;
      plainText[85] = hLP1DD;
      plainText[86] = hLP1HH;
      plainText[87] = hLP1MM;
      // lp2 amr tweaked
      plainText[88] = hLP2 >> 8;
      plainText[89] = hLP2 & 0xFF;
      // lp3
      plainText[90] = (hLP3 & 0xFF000000) >> 24;
      plainText[91] = (hLP3 & 0x00FF0000) >> 16;
      plainText[92] = (hLP3 & 0x0000FF00) >> 8;
      plainText[93] = (hLP3 & 0X000000FF);

    }  // eof 7001

    if (loraIndex == 1) {
      //
      uint32_t fPort32 = 7002;

      plainText[0] = fPort32 >> 8;
      plainText[1] = fPort32 & 0xFF;
      //
      plainText[2] = (devID & 0xFF000000) >> 24;
      plainText[3] = (devID & 0x00FF0000) >> 16;
      plainText[4] = (devID & 0x0000FF00) >> 8;
      plainText[5] = (devID & 0X000000FF);
      //
      plainText[6] = hanCode;
      // LP test
      plainText[7] = hLP1YY >> 8;
      plainText[8] = hLP1YY & 0xFF;
      plainText[9] = hLP1MT;
      plainText[10] = hLP1DD;
      plainText[11] = hLP1HH;
      plainText[12] = hLP1MM;
      // lp2 amr tweaked
      plainText[13] = hLP2 >> 8;
      plainText[14] = hLP2 & 0xFF;
      // lp3
      plainText[15] = (hLP3 & 0xFF000000) >> 24;
      plainText[16] = (hLP3 & 0x00FF0000) >> 16;
      plainText[17] = (hLP3 & 0x0000FF00) >> 8;
      plainText[18] = (hLP3 & 0X000000FF);

    }  // eof 7002

    // encrypt
    byte cipherText[CHA_CHA_POLY_MESSAGE_SIZE];
    byte tag[CHA_CHA_POLY_TAG_SIZE];
    ChaChaPolyCipher.encrypt(key, iv, auth, plainText,
                             cipherText, tag);

    // verify
    bool verify = ChaChaPolyCipher.decrypt(
        key, iv, auth, cipherText, plainText, tag);

    if (verify) {
      byte txArray[CHA_CHA_POLY_MESSAGE_SIZE +
                   CHA_CHA_POLY_TAG_SIZE +
                   CHA_CHA_POLY_IV_SIZE];

      memcpy(txArray, cipherText, sizeof(cipherText));
      memcpy(&txArray[sizeof(cipherText)], tag,
             sizeof(tag));
      memcpy(&txArray[sizeof(cipherText) + sizeof(tag)],
             iv, sizeof(iv));

      LoRa.beginPacket();
      LoRa.write(txArray, sizeof(txArray));
      LoRa.endPacket();
    }  // end if verify true

    loraRead = millis();
    loraWork = false;
    loraIndex++;

    if (loraIndex > 10) {
      loraIndex = 1;
    }

  }  // end if loraWork

}  // end loop

// EOF
