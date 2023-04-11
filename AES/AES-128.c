#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <openssl/aes.h>

#define Nb 4        // Number of columns (32-bit words) comprising the State
#define Nk 4        // Number of 32-bit words comprising the Cipher Key
#define Nr 10       // Number of rounds in AES Cipher
#define xtime(a) ( ((a) & 0x80) ? (((a) << 1) ^ 0x1b) : ((a) << 1) )

// AES S-box
static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};


static const uint8_t Rcon[] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

// AES state
typedef uint8_t state_t[4][4];


// AES key schedule
void KeyExpansion(const uint8_t *key, uint8_t *exp_key)
{
    uint32_t i, j, k, r;
    uint8_t temp[4], temp2[4];
    uint8_t round_const;
    // The first Nk words of the expanded key are simply the key itself
    for (i = 0; i < Nk; ++i)
    {
        exp_key[4*i] = key[4*i];
        exp_key[4*i+1] = key[4*i+1];
        exp_key[4*i+2] = key[4*i+2];
        exp_key[4*i+3] = key[4*i+3];
    }

    // Generate the remaining words of the expanded key
    for (i = Nk; i < Nb * (Nr+1); ++i)
    {
        // Copy the previous word
        for (j = 0; j < 4; ++j)
            temp[j] = exp_key[4*(i-1)+j];
        
        // Perform key schedule core
        if (i % Nk == 0)
        {
            // Rotate word
            temp2[0] = sbox[temp[1]] ^ Rcon[i/Nk];
            temp2[1] = sbox[temp[2]];
            temp2[2] = sbox[temp[3]];
            temp2[3] = sbox[temp[0]];
            round_const = Rcon[i/Nk];
        }
        else if (Nk > 6 && i % Nk == 4)
        {
            // Substitute word
            temp2[0] = sbox[temp[0]];
            temp2[1] = sbox[temp[1]];
            temp2[2] = sbox[temp[2]];
            temp2[3] = sbox[temp[3]];
        }
        else
        {
            // No key schedule core operation
            temp2[0] = temp[0];
            temp2[1] = temp[1];
            temp2[2] = temp[2];
            temp2[3] = temp[3];
        }
        
        // XOR with previous word Nk positions away
        k = 4*i - Nk*4;
        exp_key[4*i+0] = exp_key[k+0] ^ temp2[0];
        exp_key[4*i+1] = exp_key[k+1] ^ temp2[1];
        exp_key[4*i+2] = exp_key[k+2] ^ temp2[2];
        exp_key[4*i+3] = exp_key[k+3] ^ temp2[3];
    }
}

// AES SubBytes transformation
void SubBytes(state_t *state)
{
    uint8_t i, j;
    for (i = 0; i < 4; ++i)
    for (j = 0; j < 4; ++j)
        (*state)[i][j] = sbox[(*state)[i][j]];
}

// AES ShiftRows transformation
void ShiftRows(state_t *state)
{
    uint8_t temp;
    // Row 1
    temp = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = temp;

    // Row 2
    temp = (*state)[2][0];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][2] = temp;
    temp = (*state)[2][1];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][3] = temp;

    // Row 3
    temp = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = (*state)[3][0];
    (*state)[3][0] = temp;
}


// AES MixColumns transformation
void MixColumns(state_t *state)
{
    uint8_t i;
    uint8_t temp, temp2, h;
    for (i = 0; i < 4; ++i)
    {
        h = (*state)[i][0];
        temp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3];
        temp2 = (*state)[i][0] ^ (*state)[i][1];
        temp2 = xtime(temp2);
        (*state)[i][0] ^= temp ^ temp2;
        temp2 = (*state)[i][1] ^ (*state)[i][2];
        temp2 = xtime(temp2);
        (*state)[i][1] ^= temp ^ temp2;
        temp2 = (*state)[i][2] ^ (*state)[i][3];
        temp2 = xtime(temp2);
        (*state)[i][2] ^= temp ^ temp2;
        temp2 = (*state)[i][3] ^ h;
        temp2 = xtime(temp2);
        (*state)[i][3] ^= temp ^ temp2;
    }
}

// AES AddRoundKey transformation
void AddRoundKey(state_t *state, const uint8_t *round_key)
{
    uint8_t i, j;
    for (i = 0; i < 4; ++i)
    for (j = 0; j < 4; ++j)
        (*state)[j][i] ^= round_key[4*i+j];
}

// AES Cipher
void Cipher(const uint8_t *input, const uint8_t *exp_key, uint8_t *output)
{
    state_t state;
    uint8_t i, round;
    
    // Copy input to state array
    for (i = 0; i < 16; ++i)
        state[i%4][i/4] = input[i];
    
    // Add initial round key
    AddRoundKey(&state, exp_key);
    
    // Perform Nr-1 rounds
    for (round = 1; round < Nr; ++round)
    {
        SubBytes(&state);
        ShiftRows(&state);
        MixColumns(&state);
        AddRoundKey(&state, exp_key + round*Nb*4);
    }
    
    // Perform last round
    SubBytes(&state);
    ShiftRows(&state);
    AddRoundKey(&state, exp_key + Nr*Nb*4);
    
    // Copy state array to output
    for (i = 0; i < 16; ++i)
        output[i] = state[i%4][i/4];
}

void test_aes_128(uint8_t* input)
{
    uint8_t encrypted[16];
    uint8_t decrypted[16];
    printf("\nOpenSSL Input:\n");
    for (int i = 0; i < 16; ++i) 
    {
        printf("%02x ", input[i]);
    }
    // Encrypt using OpenSSL AES function
    AES_KEY aes_key;
    AES_KEY de_key;
    AES_set_encrypt_key(key, 128, &aes_key);
    AES_encrypt(input, encrypted, &aes_key);
    printf("\nOpenSSL Encrypted Output:\n");
    for (int i = 0; i < 16; ++i) 
    {
        printf("%02x ", encrypted[i]);
    }
    AES_set_decrypt_key(key, 128, &de_key);
    AES_decrypt(encrypted, decrypted, &de_key);
    printf("\nOpenSSL Decrypted Output:\n");
    for (int i = 0; i < 16; ++i) 
    {
        printf("%02x ", decrypted[i]);
    }
}

// Main function (example usage)
int main()
{
    uint8_t input[16] = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };
    uint8_t output[16];
    // uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t exp_key[176];
    KeyExpansion(key, exp_key);

    Cipher(input, exp_key, output);

    printf("Input:\n");
    for (int i = 0; i < 16; ++i) 
    {
        printf("%02x ", input[i]);
    }

    printf("\nOutput:\n");
    for (int i = 0; i < 16; ++i) 
    {
        printf("%02x ", output[i]);
    }

    test_aes_128(input);
    return 0;
}


