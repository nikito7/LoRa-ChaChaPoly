// han_h

// LoRa

bool loraWork = false;
uint32_t loraDelay = 11000;
uint32_t loraRead = 0;
uint8_t loraIndex = 2;  // 2 = skip LP

// HAN

uint8_t hanCFG = 99;
uint8_t hanEB = 99;
uint16_t hanERR = 0;
bool hanWork = false;
uint32_t hanDelay = 0;
uint32_t hanDelayWait = 700;
uint32_t hanDelayError = 91000;
uint8_t hanIndex = 0;  // 0 = setup
uint32_t hanRead = 0;
uint8_t hanCode = 0;

// Clock 01
uint16_t hanYY = 0;
uint8_t hanMT = 0;
uint8_t hanDD = 0;
uint8_t hanWD = 0;
uint8_t hanHH = 0;
uint8_t hanMM = 0;
uint8_t hanSS = 0;

// Voltage Current 6C
// mono
uint16_t hanVL1 = 0;
uint16_t hanCL1 = 0;
// tri
uint16_t hanVL2 = 0;
uint16_t hanCL2 = 0;
uint16_t hanVL3 = 0;
uint16_t hanCL3 = 0;
uint16_t hanCLT = 0;

// Total Energy T (kWh) 26
uint32_t hanTET1 = 0;
uint32_t hanTET2 = 0;
uint32_t hanTET3 = 0;

// Total Energy (kWh) 16
uint32_t hanTEI = 0;
uint32_t hanTEE = 0;

// Active Power Import/Export 73
// tri
uint32_t hanAPI1 = 0;
uint32_t hanAPE1 = 0;
uint32_t hanAPI2 = 0;
uint32_t hanAPE2 = 0;
uint32_t hanAPI3 = 0;
uint32_t hanAPE3 = 0;
// mono / tri total. (79)
uint32_t hanAPI = 0;
uint32_t hanAPE = 0;

// Power Factor (7B) / Frequency (7F)
uint16_t hanPF = 0;
uint16_t hanPF1 = 0;
uint16_t hanPF2 = 0;
uint16_t hanPF3 = 0;
uint16_t hanFreq = 0;

// Last Profile

uint16_t hLP1YY = 0;
uint8_t hLP1MT = 0;
uint8_t hLP1DD = 0;
uint8_t hLP1HH = 0;
uint8_t hLP1MM = 0;

uint16_t hLP2 = 0;  // tweaked to 16bits

uint32_t hLP3 = 0;
uint32_t hLP4 = 0;
uint32_t hLP5 = 0;
uint32_t hLP6 = 0;

// Misc

float hCT1 = 0;
uint8_t hTariff = 0;

// EOF
