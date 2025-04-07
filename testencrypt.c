#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE_RANGE 256
const char *key_file = "key.txt";

void generate_reflector(unsigned char reflector[BYTE_RANGE]) {
    unsigned char pool[BYTE_RANGE];
    for (int i = 0; i < BYTE_RANGE; i++) {
        pool[i] = i;
        reflector[i] = 255;
    }

    for (int i = BYTE_RANGE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned char tmp = pool[i];
        pool[i] = pool[j];
        pool[j] = tmp;
    }

    for (int i = 0; i < BYTE_RANGE; i += 2) {
        unsigned char a = pool[i];
        unsigned char b = pool[i + 1];
        reflector[a] = b;
        reflector[b] = a;
    }
}


char *encrypt(const char *password, const char *key_file_path) {
    static char hex_output[512];
    unsigned char output[256];
    unsigned char rotor1[BYTE_RANGE], rotor2[BYTE_RANGE], rotor3[BYTE_RANGE];
    unsigned char reverse1[BYTE_RANGE], reverse2[BYTE_RANGE], reverse3[BYTE_RANGE];
    unsigned char reflector[BYTE_RANGE], plugboard[BYTE_RANGE];
    int pos1 = 0, pos2 = 0, pos3 = 0;

    FILE *file = fopen(key_file_path, "r");
    if (!file) return NULL;

    unsigned int seed;
    if (fscanf(file, "%u", &seed) != 1) {
        fclose(file);
        return NULL;
    }
    fclose(file);

    srand(seed);

    for (int i = 0; i < BYTE_RANGE; i++) {
        rotor1[i] = rotor2[i] = rotor3[i] = plugboard[i] = i;
    }

    for (int i = BYTE_RANGE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned char tmp;
        tmp = rotor1[i]; rotor1[i] = rotor1[j]; rotor1[j] = tmp;
        tmp = rotor2[i]; rotor2[i] = rotor2[j]; rotor2[j] = tmp;
        tmp = rotor3[i]; rotor3[i] = rotor3[j]; rotor3[j] = tmp;
    }

    generate_reflector(reflector);

    for (int i = 0; i < BYTE_RANGE; i++) {
        reverse1[rotor1[i]] = i;
        reverse2[rotor2[i]] = i;
        reverse3[rotor3[i]] = i;
    }

    size_t len = strlen(password);
    for (size_t i = 0; i < len; i++) {
        pos1 = (pos1 + 1) % BYTE_RANGE;
        if (pos1 == 0) {
            pos2 = (pos2 + 1) % BYTE_RANGE;
            if (pos2 == 0) {
                pos3 = (pos3 + 1) % BYTE_RANGE;
            }
        }

        unsigned char b = (unsigned char)password[i];

        b = plugboard[b];
        b = rotor1[(b + pos1) % BYTE_RANGE];
        b = rotor2[(b + pos2) % BYTE_RANGE];
        b = rotor3[(b + pos3) % BYTE_RANGE];
        b = reflector[b];
        b = (reverse3[b] + BYTE_RANGE - pos3) % BYTE_RANGE;
        b = (reverse2[b] + BYTE_RANGE - pos2) % BYTE_RANGE;
        b = (reverse1[b] + BYTE_RANGE - pos1) % BYTE_RANGE;
        b = plugboard[b];

        output[i] = b;
    }

    for (size_t i = 0; i < len; i++) {
        sprintf(hex_output + i * 2, "%02X", output[i]);
    }
    hex_output[len * 2] = '\0';

    return hex_output;
}

char *decrypt(const char *encrypted_password, const char *key_file_path) {
    static char hex_output[512];
    unsigned char output[256];
    unsigned char rotor1[BYTE_RANGE], rotor2[BYTE_RANGE], rotor3[BYTE_RANGE];
    unsigned char reverse1[BYTE_RANGE], reverse2[BYTE_RANGE], reverse3[BYTE_RANGE];
    unsigned char reflector[BYTE_RANGE], plugboard[BYTE_RANGE];
    int pos1 = 0, pos2 = 0, pos3 = 0;

    FILE *file = fopen(key_file_path, "r");
    if (!file) return NULL;

    unsigned int seed;
    if (fscanf(file, "%u", &seed) != 1) {
        fclose(file);
        return NULL;
    }
    fclose(file);

    srand(seed);

    for (int i = 0; i < BYTE_RANGE; i++) {
        rotor1[i] = rotor2[i] = rotor3[i] = plugboard[i] = i;
    }

    for (int i = BYTE_RANGE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned char tmp;
        tmp = rotor1[i]; rotor1[i] = rotor1[j]; rotor1[j] = tmp;
        tmp = rotor2[i]; rotor2[i] = rotor2[j]; rotor2[j] = tmp;
        tmp = rotor3[i]; rotor3[i] = rotor3[j]; rotor3[j] = tmp;
    }

    generate_reflector(reflector);

    for (int i = 0; i < BYTE_RANGE; i++) {
        reverse1[rotor1[i]] = i;
        reverse2[rotor2[i]] = i;
        reverse3[rotor3[i]] = i;
    }

    size_t len = strlen(encrypted_password) / 2;
    for (size_t i = 0; i < len; i++) {
        sscanf(encrypted_password + i * 2, "%2hhX", &output[i]);
    }

    for (size_t i = 0; i < len; i++) {
        pos1 = (pos1 + 1) % BYTE_RANGE;
        if (pos1 == 0) {
            pos2 = (pos2 + 1) % BYTE_RANGE;
            if (pos2 == 0) {
                pos3 = (pos3 + 1) % BYTE_RANGE;
            }
        }

        unsigned char b = output[i];

        b = plugboard[b];
        b = rotor1[(b + pos1) % BYTE_RANGE];
        b = rotor2[(b + pos2) % BYTE_RANGE];
        b = rotor3[(b + pos3) % BYTE_RANGE];
        b = reflector[b];
        b = (reverse3[b] + BYTE_RANGE - pos3) % BYTE_RANGE;
        b = (reverse2[b] + BYTE_RANGE - pos2) % BYTE_RANGE;
        b = (reverse1[b] + BYTE_RANGE - pos1) % BYTE_RANGE;
        b = plugboard[b];

        output[i] = b;
    }

    for (size_t i = 0; i < len; i++) {
        hex_output[i] = (char)output[i];
    }
    hex_output[len] = '\0';

    return hex_output;
}

int main() {
    const char *testpassword = "1111";
    printf("Password = %s\n", testpassword);

    char *encrypted_password = encrypt(testpassword, key_file);
    if (encrypted_password == NULL) {
        printf("Encryption failed!\n");
        return 1;
    }
    printf("Encrypted password: %s\n", encrypted_password);

    char *decrypted_password = decrypt(encrypted_password, key_file);
    if (decrypted_password == NULL) {
        printf("Decryption failed!\n");
        return 1;
    }
    printf("Decrypted password: %s\n", decrypted_password);

    return 0;
}
