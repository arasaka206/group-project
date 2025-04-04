#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100
#define SHIFT 3

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
