#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LEN 100
#define SHIFT 3
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
//_____________________________________________________


void caesarEncrypt(char *str) {
    int i = 0;
    while (str[i] != '\0' && str[i] != '\n') {
        str[i] += SHIFT;
        i++;
    }
}

void caesarDecrypt(char *str) {
    int i = 0;
    while (str[i] != '\0' && str[i] != '\n') {
        str[i] -= SHIFT;
        i++;
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
    caesarEncrypt(targetUser); // so we can compare encoded username

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

            caesarEncrypt(newUser);
            caesarEncrypt(newPass);

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

            caesarEncrypt(password);
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
            caesarDecrypt(storedPass);

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
                    caesarEncrypt(newUser);
                    caesarEncrypt(newPass);

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
                        caesarDecrypt(value);
                        printf("%s: %s\n", label, value);
                    }

                    fclose(fp);}
                else if(walletOption = 3){
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


