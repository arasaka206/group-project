#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_LEN 100
#define SHIFT 3
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

int check_password_strength(const char *password) {
    int score = 0;
    int length = strlen(password);

    // Points based on length
    if (length >= 8) {
        score++;  // Basic length criteria met
    }
    if (length >= 12) {
        score++;  // Extra points for longer passwords
    }

    // Flags for different types of characters
    int hasLower = 0, hasUpper = 0, hasDigit = 0, hasSpecial = 0;

    // Check each character in the password
    for (int i = 0; i < length; i++) {
        if (islower(password[i]))
            hasLower = 1;
        else if (isupper(password[i]))
            hasUpper = 1;
        else if (isdigit(password[i]))
            hasDigit = 1;
        else
            hasSpecial = 1;
    }

    // Add points for each category found
    score += hasLower + hasUpper + hasDigit + hasSpecial;

    return score;
}

void print_strength_message(char *password) {
    int strength = check_password_strength(password);
    int answer;

    if (strength <= 2) {
        printf("Password Strength: Weak\n");
        printf("Do you want to keep this password? (0: Yes, 1: No): ");
        scanf("%d", &answer);
        getchar(); // consume the leftover newline

        if (answer == 1) {
            printf("Enter new password: ");
            fgets(password, MAX_LEN, stdin);
            password[strcspn(password, "\n")] = '\0'; // remove newline
            strength = check_password_strength(password);
            print_strength_message(password);

        } else { return;
        }
    }

    else if (strength <= 4) {
        printf("Password Strength: Moderate\n");
        return;
    } else {
        printf("Password Strength: Strong\n");
        return;
    }
}

void exit_program() {
    printf("Exiting program. Goodbye!\n");
    exit(0);
}

void update_credentials(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Could not open wallet for reading.\n");
        return;
    }

    char lines[100][MAX_LEN];  // store all lines temporarily
    int lineCount = 0;
    char label[MAX_LEN], value[MAX_LEN];

    // Read file into memory
    while (fscanf(fp, "%[^:]:%s\n", label, value) != EOF) {
        snprintf(lines[lineCount], MAX_LEN, "%s:%s", label, value);
        lineCount++;
    }
    fclose(fp);

    char targetUser[MAX_LEN];
    printf("Enter the username you want to update: ");
    scanf("%s", targetUser);
    strcpy(targetUser, encrypt(targetUser, key_file)); // encrypt username for comparison

    int found = 0;

    for (int i = 0; i < lineCount; i++) {
        if (strncmp(lines[i], "USER:", 5) == 0 && strcmp(lines[i] + 5, targetUser) == 0) {
            found = 1;

            char newUser[MAX_LEN], newPass[MAX_LEN];
            printf("Enter new username: ");
            scanf("%s", newUser);
            printf("Enter new password: ");
            scanf("%s", newPass);

            print_strength_message(newPass);

            strcpy(newUser, encrypt(newUser, key_file));
            strcpy(newPass, encrypt(newPass, key_file));

            // Update the USER and PASS lines
            snprintf(lines[i], MAX_LEN, "USER:%s", newUser);
            snprintf(lines[i+1], MAX_LEN, "PASS:%s", newPass); // assumes USER is always followed by PASS

            break;
        }
    }

    if (!found) {
        printf("Username not found.\n");
        return;
    }

    // Write back all data
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Could not open wallet for writing.\n");
        return;
    }

    for (int i = 0; i < lineCount; i++) {
        fprintf(fp, "%s\n", lines[i]);
    }

    fclose(fp);
    printf("Credentials updated successfully.\n");
}

int main() {
    char username[MAX_LEN], password[MAX_LEN];
    char inputUser[MAX_LEN], inputPass[MAX_LEN];
    char filename[MAX_LEN];
    FILE *fp;
    int loggedIn = 0;
    //____________________________
    time_t lastActivity = 0;

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

            sprintf(filename, "%s_wallet.txt", username);

            fp = fopen(filename, "w");
            if (fp == NULL) {
                printf("Error creating file.\n");
                continue;
            }

            strcpy(password, encrypt(password, key_file));
            fprintf(fp, "PASSWORD:%s\n", password);
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
            strcpy(storedPass, decrypt(storedPass, key_file));

            if (strcmp(storedPass, inputPass) == 0) {
                printf("Login successful!\n");
                loggedIn = 1;

                lastActivity = time(NULL);
            } else {
                printf("Incorrect password.\n");
                fclose(fp);
                continue;
            }

            while (loggedIn) {
            //_________________________________
            time_t now = time(NULL);
        if (difftime(now, lastActivity) > 60) { // 60 sec timeout
        printf("\n[Timeout] App locked due to inactivity.\n");

        char reInput[MAX_LEN];
        printf("Re-enter master password: ");
        scanf("%s", reInput);

        if (strcmp(reInput, storedPass) != 0) {
            printf("Incorrect password. Logging out...\n");
            loggedIn = 0;
            break;
        } else {
            printf("Unlocked successfully.\n");
            lastActivity = time(NULL);
        }
    }
    //_____________________________________
                printf("\n--- Wallet Menu ---\n");
                printf("1. Add new username and password\n");
                printf("2. View wallet\n");
                printf("3. Update a Credential\n");
                printf("4. Logout\n");
                printf("Choose option (1-4): ");
                int walletOption;
                scanf("%d", &walletOption);
                //_______________________________
                lastActivity = time(NULL); // Reset timeout clock
                //________________________________
                if (walletOption == 1) {
                    char newUser[MAX_LEN], newPass[MAX_LEN];
                    printf("Enter new username: ");
                    scanf("%s", newUser);
                    printf("Enter new password: ");
                    scanf("%s", newPass);
                    //_____________________________________
                    print_strength_message(newPass);
                    //________________________________________
                    strcpy(newUser, encrypt(newUser, key_file));
                    strcpy(newPass, encrypt(newPass, key_file));

                    fp = fopen(filename, "a");
                    if (fp == NULL) {
                        printf("Error opening file.\n");
                        continue;
                    }

                    fprintf(fp, "USER:%s\n", newUser);
                    fprintf(fp, "PASS:%s\n", newPass);
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
                        strcpy(value, decrypt(value, key_file));
                        printf("%s: %s\n", label, value);
                    }

                    fclose(fp);}
                else if(walletOption == 3){
                    update_credentials(filename);

                } else if (walletOption == 4) {
                    printf("Logged out.\n");
                    loggedIn = 0;
                } else {
                    printf("Invalid choice.\n");
                }
            }
    //_______________________________
        } else if (option == 3) {
            exit_program();
        } else {
            printf("Invalid option. Please try again.\n");
        }
    }

    return 0;}
