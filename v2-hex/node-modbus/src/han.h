// han.h

uint8_t hanCNT = 0;
uint8_t hanCFG = 99;
uint8_t hanEB = 99;
uint16_t hanERR = 0;
uint16_t hanDTT = 0;
bool hanWork = true;
uint32_t hanDelay = 0;
uint32_t hanDelayWait = 900;
uint32_t hanDelayError = 66666;
uint8_t hanIndex = 1;
uint32_t lastRead = 0;
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

// EOF
