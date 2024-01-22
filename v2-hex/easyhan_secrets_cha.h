// ChaChaPoly secrets

#define CHA_CHA_POLY_KEY_SIZE 32
#define CHA_CHA_POLY_IV_SIZE 12
#define CHA_CHA_POLY_AUTH_SIZE 16
#define CHA_CHA_POLY_TAG_SIZE 16
#define CHA_CHA_POLY_MESSAGE_SIZE 100

byte key[CHA_CHA_POLY_KEY_SIZE] = {
    0x3c, 0x36, 0x23, 0xcc, 0xc0, 0xc6, 0x7c, 0xbe,
    0xbf, 0x63, 0x98, 0x43, 0x1d, 0x1b, 0xb9, 0xcf,
    0xe5, 0xa6, 0xb5, 0xc2, 0xb9, 0x33, 0xb8, 0xc6,
    0xc6, 0x06, 0xe9, 0xbe, 0xa4, 0xc2, 0x62, 0x37};

byte auth[CHA_CHA_POLY_AUTH_SIZE] = {
    0xa4, 0x5e, 0x7c, 0x2f, 0xb2, 0x20, 0x0a, 0xf2,
    0x2e, 0xfd, 0xff, 0x91, 0x65, 0x54, 0xba, 0x0a};

// EOF
