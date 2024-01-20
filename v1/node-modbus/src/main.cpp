// LoRa Modbus Node
#include <LoRa.h>
#include <ArduinoJson.h>
#include <ModbusMaster.h>
#include <HardwareSerial.h>

// esp8266
#define LORA_SCK 14
#define LORA_MISO 12
#define LORA_MOSI 13

#define LORA_SS 15
#define LORA_RST -1
#define LORA_DI0 5

// ### CHA_CHA_POLY ###

#include "ChaChaPolyHelper.h"

#include "../../easyhan_secrets_cha.h"

byte iv[CHA_CHA_POLY_IV_SIZE];

// ### CHA_CHA_POLY EOF ###

#include "han.h"

ModbusMaster node;

void hanBlink()
{
    #ifdef ESP8266
    digitalWrite(2, LOW);
    delay(150);
    digitalWrite(2, HIGH);
    #endif
}

void setup()
{
    delay(2000);

    #ifdef ESP8266
    pinMode(2, OUTPUT);
    #endif

    HardwareSerial& serial = Serial;

    delay(1000);

    Serial.begin(9600, SERIAL_8N1);

    while (!Serial);

    SPI.begin();
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
    LoRa.enableCrc();
    LoRa.setSyncWord(0x12);

    if (!LoRa.begin(868E6)) {
      while (1);
    }

    node.begin(1, Serial);

    uint8_t testserial;

    testserial = node.readInputRegisters(0x0001, 1);
    if (testserial == node.ku8MBSuccess)
    {
      hanCFG = 1;
    }
    else
    {
      Serial.end();
      delay(500);
      Serial.begin(9600, SERIAL_8N2);
      hanCFG = 2;
    }

    delay(1000);

    // Detect EB Type

    testserial = node.readInputRegisters(0x0070, 2);
    if (testserial == node.ku8MBSuccess)
    {
      // 
      hanDTT = node.getResponseBuffer(0);
      if (hanDTT > 0)
      {
        hanEB = 3;
      }
      else
      {
        hanEB = 1;
      }
      // 
    }
    else
    {
      hanEB = 1;
    }

} // end setup

void loop()
{
    float up = millis() / 1000;

    // # # # # # # # # # #
    // EASYHAN MODBUS BEGIN
    // # # # # # # # # # #

    uint8_t result;

    // # # # # # # # # # #
    // Clock ( 12 bytes )
    // # # # # # # # # # #

    result = node.readInputRegisters(0x0001, 1);
    if (result == node.ku8MBSuccess)
    {
      hanBlink();
      hanYY = node.getResponseBuffer(0);
      hanMT = node.getResponseBuffer(1) >> 8;
      hanDD = node.getResponseBuffer(1) & 0xFF;
      hanWD = node.getResponseBuffer(2) >> 8;
      hanHH = node.getResponseBuffer(2) & 0xFF;
      hanMM = node.getResponseBuffer(3) >> 8;
      hanSS = node.getResponseBuffer(3) & 0xFF;
    }
    else
    {
      hanERR++;
    }
    delay(1000);

    // # # # # # # # # # #
    // Voltage Current
    // # # # # # # # # # #

    if (hanEB == 3)
    {
      result = node.readInputRegisters(0x006c, 7);
      if (result == node.ku8MBSuccess)
      {
        hanBlink();
        hanVL1 = node.getResponseBuffer(0);
        hanCL1 = node.getResponseBuffer(1);
        hanVL2 = node.getResponseBuffer(2);
        hanCL2 = node.getResponseBuffer(3);
        hanVL3 = node.getResponseBuffer(4);
        hanCL3 = node.getResponseBuffer(5);
        hanCLT = node.getResponseBuffer(6);
      }
      else
      {
      hanERR++;
      }
    }
    else
    {
      result = node.readInputRegisters(0x006c, 2);
      if (result == node.ku8MBSuccess)
      {
        hanBlink();
        hanVL1 = node.getResponseBuffer(0);
        hanCL1 = node.getResponseBuffer(1);
      }
      else
      {
        hanERR++;
      }
    }
    delay(1000);

    // # # # # # # # # # #
    // Active Power Import/Export 73 (tri)
    // Power Factor (mono) (79..)
    // # # # # # # # # # #

    if (hanEB == 3)
    {
      result = node.readInputRegisters(0x0073, 8);
      if (result == node.ku8MBSuccess)
      {
        hanBlink();
        hanAPI1 = node.getResponseBuffer(1) | node.getResponseBuffer(0) << 16;
        hanAPE1 = node.getResponseBuffer(3) | node.getResponseBuffer(2) << 16;
        hanAPI2 = node.getResponseBuffer(5) | node.getResponseBuffer(4) << 16;
        hanAPE2 = node.getResponseBuffer(7) | node.getResponseBuffer(6) << 16;
        hanAPI3 = node.getResponseBuffer(9) | node.getResponseBuffer(8) << 16;
        hanAPE3 = node.getResponseBuffer(11) | node.getResponseBuffer(10) << 16;
        hanAPI = node.getResponseBuffer(13) | node.getResponseBuffer(12) << 16;
        hanAPE = node.getResponseBuffer(15) | node.getResponseBuffer(14) << 16;
      }
      else
      {
        hanERR++;
      }
    }
    else
    {
      result = node.readInputRegisters(0x0079, 3);
      if (result == node.ku8MBSuccess)
      {
        hanBlink();
        hanAPI = node.getResponseBuffer(1) | node.getResponseBuffer(0) << 16;
        hanAPE = node.getResponseBuffer(3) | node.getResponseBuffer(2) << 16;
        hanPF = node.getResponseBuffer(4);
      }
      else
      {
        hanERR++;
      }
    }
    delay(1000);

    // # # # # # # # # # #
    // Power Factor (7B) / Frequency (7F)
    // Power Factor (tri)
    // Frequency (mono)
    // # # # # # # # # # #

    if (hanEB == 3)
    {
      result = node.readInputRegisters(0x007b, 5);
      if (result == node.ku8MBSuccess)
      {
        hanBlink();
        hanPF = node.getResponseBuffer(0);
        hanPF1 = node.getResponseBuffer(1);
        hanPF2 = node.getResponseBuffer(2);
        hanPF3 = node.getResponseBuffer(3);
        hanFreq = node.getResponseBuffer(4);
      }
      else
      {
        hanERR++;
      }
    }
    else
    {
      result = node.readInputRegisters(0x007f, 1);
      if (result == node.ku8MBSuccess)
      {
        hanBlink();
        hanFreq = node.getResponseBuffer(0);
      }
      else
      {
        hanERR++;
      }
    }
    delay(1000); 

    // # # # # # # # # # #
    // Total Energy Tarifas (kWh) 26
    // # # # # # # # # # #

    result = node.readInputRegisters(0x0026, 3);
    if (result == node.ku8MBSuccess)
    {
      hanBlink();
      hanTET1 = node.getResponseBuffer(1) | node.getResponseBuffer(0) << 16;
      hanTET2 = node.getResponseBuffer(3) | node.getResponseBuffer(2) << 16;
      hanTET3 = node.getResponseBuffer(5) | node.getResponseBuffer(4) << 16;
    }
    else
    {
      hanERR++;
    }
    delay(1000);

    // # # # # # # # # # #
    // Total Energy (total) (kWh) 16
    // # # # # # # # # # #

    result = node.readInputRegisters(0x0016, 2);
    if (result == node.ku8MBSuccess)
    {
      hanBlink();
      hanTEI = node.getResponseBuffer(1) | node.getResponseBuffer(0) << 16;
      hanTEE = node.getResponseBuffer(3) | node.getResponseBuffer(2) << 16;
    }
    else
    {
      hanERR++;
    }
    delay(1000);

    // # # # # # # # # # #
    // EASYHAN MODBUS EOF
    // # # # # # # # # # #

    if (hanERR > 900 )
    {
      hanERR = 0;
    }

    // json

    static char jsonBuffer[100];
    StaticJsonDocument<100> doc;

    doc["id"] = "LoRa_cpha8";
    doc["EBx"] = hanEB;
    doc["Ser"] = hanCFG;
    doc["ERR"] = hanERR;
    doc["up"] = up;

    size_t len = measureJson(doc) + 1;
    serializeJson(doc, jsonBuffer, len);

    // ### CHA_CHA_POLY ###

    ChaChaPolyCipher.generateIv(iv);

    byte plainText[CHA_CHA_POLY_MESSAGE_SIZE];
    String plain = jsonBuffer;
    plain.getBytes(plainText, CHA_CHA_POLY_MESSAGE_SIZE);

    byte cipherText[CHA_CHA_POLY_MESSAGE_SIZE];
    byte tag[CHA_CHA_POLY_TAG_SIZE];
    ChaChaPolyCipher.encrypt(key, iv, auth, plainText, cipherText, tag);

    bool verify = ChaChaPolyCipher.decrypt(key, iv, auth, cipherText, plainText, tag);

    if (verify)
    {
      byte txArray[CHA_CHA_POLY_MESSAGE_SIZE + 
                   CHA_CHA_POLY_TAG_SIZE +
                   CHA_CHA_POLY_IV_SIZE];

      memcpy(txArray, cipherText, sizeof(cipherText));
      memcpy(&txArray[sizeof(cipherText)], tag, sizeof(tag));
      memcpy(&txArray[sizeof(cipherText) +
                      sizeof(tag)], iv, sizeof(iv));

      // ### CHA_CHA_POLY EOF ###

      LoRa.beginPacket();
      LoRa.write(txArray, sizeof(txArray));
      LoRa.endPacket();

    } // end if verify true

    delay(11000);
}

// EOF
