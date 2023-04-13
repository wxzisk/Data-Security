#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <openssl/aes.h>

// default key
const uint8_t key_128[16]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

const uint8_t key_192[24]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                           0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

const uint8_t key_256[32]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                           0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};


void test_aes_128(uint8_t* input)
{
    uint8_t encrypted[16];
    uint8_t decrypted[16];
    // Encrypt using OpenSSL AES function
    AES_KEY aes_key;
    AES_KEY de_key;
    AES_set_encrypt_key(key_128, 128, &aes_key);
    AES_encrypt(input, encrypted, &aes_key);
    printf("----------------AES-128----------------");
    printf("\nOpenSSL Encrypted Output:\n");
    for (int i = 0; i < 16; ++i) 
    {
        printf("%02x ", encrypted[i]);
    }
    printf("\n");
}

void test_aes_192(uint8_t* input)
{
    uint8_t encrypted[16];
    uint8_t decrypted[16];
    // Encrypt using OpenSSL AES function
    AES_KEY aes_key;
    AES_KEY de_key;
    AES_set_encrypt_key(key_192, 192, &aes_key);
    AES_encrypt(input, encrypted, &aes_key);
    printf("----------------AES-192----------------");
    printf("\nOpenSSL Encrypted Output:\n");
    for (int i = 0; i < 16; ++i) 
    {
        printf("%02x ", encrypted[i]);
    }
    printf("\n");
}

void test_aes_256(uint8_t* input)
{
    uint8_t encrypted[16];
    uint8_t decrypted[16];
    // Encrypt using OpenSSL AES function
    AES_KEY aes_key;
    AES_KEY de_key;
    AES_set_encrypt_key(key_256, 256, &aes_key);
    AES_encrypt(input, encrypted, &aes_key);
    printf("----------------AES-256----------------");
    printf("\nOpenSSL Encrypted Output:\n");
    for (int i = 0; i < 16; ++i) 
    {
        printf("%02x ", encrypted[i]);
    }
    printf("\n");
}


int main()
{
    uint8_t input[16] = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };
    uint8_t output[16];
    
    printf("Input:\n");
    for (int i = 0; i < 16; ++i) 
    {
        printf("%02x ", input[i]);
    }
    printf("\n");
    test_aes_128(input);
    test_aes_192(input);
    test_aes_256(input);
    return 0;
}
