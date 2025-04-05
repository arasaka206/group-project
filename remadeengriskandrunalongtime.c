#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100
#define SHIFT 3

#define BYTE_RANGE 256
const char *key_file = "key.txt";
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
        rotor1[i] = rotor2[i] = rotor3[i] = reflector[i] = plugboard[i] = i;
    }
    for (int i = BYTE_RANGE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned char tmp;

        tmp = rotor1[i]; rotor1[i] = rotor1[j]; rotor1[j] = tmp;
        tmp = rotor2[i]; rotor2[i] = rotor2[j]; rotor2[j] = tmp;
        tmp = rotor3[i]; rotor3[i] = rotor3[j]; rotor3[j] = tmp;
        tmp = reflector[i]; reflector[i] = reflector[j]; reflector[j] = tmp;
    }

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
        rotor1[i] = rotor2[i] = rotor3[i] = reflector[i] = plugboard[i] = i;
    }
    for (int i = BYTE_RANGE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned char tmp;

        tmp = rotor1[i]; rotor1[i] = rotor1[j]; rotor1[j] = tmp;
        tmp = rotor2[i]; rotor2[i] = rotor2[j]; rotor2[j] = tmp;
        tmp = rotor3[i]; rotor3[i] = rotor3[j]; rotor3[j] = tmp;
        tmp = reflector[i]; reflector[i] = reflector[j]; reflector[j] = tmp;
    }

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
        unsigned char b = output[i];

        pos1 = (pos1 + 1) % BYTE_RANGE;
        if (pos1 == 0) {
            pos2 = (pos2 + 1) % BYTE_RANGE;
            if (pos2 == 0) {
                pos3 = (pos3 + 1) % BYTE_RANGE;
            }
        }

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

void exit_program() {
    printf("Exiting program. Goodbye!\n");
    exit(0);
}

int main() {
    char username[MAX_LEN], password[MAX_LEN];
    char inputUser[MAX_LEN], inputPass[MAX_LEN];
    char filename[MAX_LEN];
    FILE *fp;
    int loggedIn = 0;

    while (1) {
        printf("\nWelcome to the Password Wallet\n");
        printf("1. New User\n");
        printf("2. Existing User\n");
        printf("3. Exit\n");
        printf("Choose option (1-3): ");

        int option;
        scanf("%d", &option);

        if (option == 1) {
            printf("Create your username (no spaces): ");
            scanf("%s", username);
            printf("Create your password (no spaces): ");
            scanf("%s", password);

            fp = fopen(filename, "w");
            if (fp == NULL) {
                printf("Error creating file.\n");
                continue;
            }

            char* encrypted_password = encrypt(password, key_file);
            sprintf(filename, "%s_wallet.txt", username);
            fprintf(fp, "PASSWORD:%s\n", encrypted_password);
            fclose(fp);
            printf("Account created successfully!\n");

        } else if (option == 2) {
            printf("Enter your username: ");
            scanf("%s", username);
            printf("Enter your password: ");
            scanf("%s", inputPass);

            sprintf(filename, "%s_wallet.txt", username);

            fp = fopen(filename, "r");
            if (fp == NULL) {
                printf("Wallet file not found.\n");
                continue;
            }

            char storedPass[MAX_LEN];
            fscanf(fp, "PASSWORD:%s\n", storedPass);
            char* decrypted_password = decrypt(storedPass, "key.txt");

            if (strcmp(decrypted_password, inputPass) == 0) {
                printf("Login successful!\n");
                loggedIn = 1;
            } else {
                printf("Incorrect password.\n");
                fclose(fp);
                continue;
            }

            while (loggedIn) {
                printf("\n--- Wallet Menu ---\n");
                printf("1. Add new username and password\n");
                printf("2. View wallet\n");
                printf("3. Logout\n");
                printf("Choose option (1-3): ");
                int walletOption;
                scanf("%d", &walletOption);

                if (walletOption == 1) {
                    char newUser[MAX_LEN], newPass[MAX_LEN];
                    printf("Enter new username: ");
                    scanf("%s", newUser);
                    printf("Enter new password: ");
                    scanf("%s", newPass);

                    char* newPass_encrypted = encrypt(newPass, key_file);

                    fp = fopen(filename, "a");
                    if (fp == NULL) {
                        printf("Error opening file.\n");
                        continue;
                    }

                    fprintf(fp, "USER:%s\n", newUser);
                    fprintf(fp, "PASS:%s\n", newPass_encrypted);
                    fclose(fp);

                    printf("Credentials added.\n");

                } else if (walletOption == 2) {
                    fp = fopen(filename, "r");
                    if (fp == NULL) {
                        printf("Error reading file.\n");
                        continue;
                    }

                    char label[MAX_LEN], value[MAX_LEN];
                    fscanf(fp, "%s", label); // skip PASSWORD line
                    fscanf(fp, "%s", value);

                    printf("\n--- Wallet Contents ---\n");

                    while (fscanf(fp, "%[^:]:%s\n", label, value) != EOF) {
                        char* decrypted_value = decrypt(value,"key.txt");
                        printf("%s: %s\n", label, decrypted_value);
                    }

                    fclose(fp);

                } else if (walletOption == 3) {
                    printf("Logged out.\n");
                    loggedIn = 0;
                } else {
                    printf("Invalid choice.\n");
                }
            }

        } else if (option == 3) {
            exit_program();
        } else {
            printf("Invalid option. Please try again.\n");
        }
    }

    return 0;
}
