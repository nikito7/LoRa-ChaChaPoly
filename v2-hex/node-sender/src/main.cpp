#include <LoRa.h>

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

int i;

void printHex(uint8_t num) {
  char hexCar[2];

  sprintf(hexCar, "%02X", num);
  Serial.print(hexCar);
}

// ### CHA_CHA_POLY EOF ###

void setup()
{
    delay(1000);

    #ifdef ESP8266
    pinMode(2, OUTPUT);
    #endif

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
}

void loop()
{
    uint32_t up = millis() / 1000;
    uint8_t fPort = 0x01;

    // ### CHA_CHA_POLY ###

    ChaChaPolyCipher.generateIv(iv);

    byte plainText[CHA_CHA_POLY_MESSAGE_SIZE];

    // clear array
    for (int i = 0; i < sizeof(plainText); i++) {
      plainText[i] = 0x77;
    }

    // hex data sensors
    plainText[0] = 0x77;
    plainText[1] = fPort;

    plainText[2] = (up & 0xFF000000) >> 24;
    plainText[3] = (up & 0x00FF0000) >> 16;
    plainText[4] = (up & 0x0000FF00) >> 8;
    plainText[5] = (up & 0X000000FF);

    // encrypt plain text message from plainText to cipherText
    byte cipherText[CHA_CHA_POLY_MESSAGE_SIZE];
    byte tag[CHA_CHA_POLY_TAG_SIZE];
    ChaChaPolyCipher.encrypt(key, iv, auth, plainText, cipherText, tag);

    Serial.println(" ");
    Serial.println("Encrypted:");

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

    // decrypt message from cipherText to plainText
    // output is valid only if result is true
    bool verify = ChaChaPolyCipher.decrypt(key, iv, auth, cipherText, plainText, tag);

    if (verify)
    {
      Serial.println(" ");
      Serial.println("Decrypted:");  
      for(i=0; i<sizeof(plainText); i++)
      {
        printHex(plainText[i]);
      }
    }

    byte txArray[CHA_CHA_POLY_MESSAGE_SIZE + 
                 CHA_CHA_POLY_TAG_SIZE +
                 CHA_CHA_POLY_IV_SIZE];

    memcpy(txArray, cipherText, sizeof(cipherText));
    memcpy(&txArray[sizeof(cipherText)], tag, sizeof(tag));
    memcpy(&txArray[sizeof(cipherText) +
                    sizeof(tag)], iv, sizeof(iv));

    Serial.println(" ");
    Serial.println("txArray:");

    for(i=0; i<sizeof(txArray); i++)
    {
      printHex(txArray[i]);
    }

    // ### CHA_CHA_POLY EOF ###

    LoRa.beginPacket();
    LoRa.write(txArray, sizeof(txArray));
    LoRa.endPacket();

    delay(10000);
}

// EOF
