#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100
#define SHIFT 3 // Mã hóa Ceasar dịch 3 ký tự

// Hàm mã hóa Ceasar
void caesarEncrypt(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] != '\n') // tránh mã hóa kí tự newline
            str[i] += SHIFT;
    }
}

// Hàm giải mã Ceasar
void caesarDecrypt(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] != '\n')
            str[i] -= SHIFT;
    }
}

int main() {
    char username[MAX_LEN], password[MAX_LEN], filename[MAX_LEN];
    char inputUser[MAX_LEN], inputPass[MAX_LEN];
    FILE *fp;

    printf("Ban la nguoi dung moi hay cu? (moi/cu): ");
    char status[10];
    fgets(status, sizeof(status), stdin);
    status[strcspn(status, "\n")] = 0; // xóa newline

    printf("Nhap ten nguoi dung: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    snprintf(filename, sizeof(filename), "%s_wallet.txt", username);

    if (strcmp(status, "moi") == 0) {
        printf("Tao mat khau: ");
        fgets(password, sizeof(password), stdin);
        password[strcspn(password, "\n")] = 0;

        // Lưu mật khẩu vào file
        fp = fopen(filename, "w");
        if (!fp) {
            perror("Khong the tao file");
            return 1;
        }

        caesarEncrypt(password);
        fprintf(fp, "PASSWORD:%s\n", password);
        fclose(fp);

        printf("Da tao vi thanh cong!\n");

    } else if (strcmp(status, "cu") == 0) {
        fp = fopen(filename, "r");
        if (!fp) {
            printf("Khong tim thay vi cua nguoi dung nay.\n");
            return 1;
        }

        // Đọc dòng đầu tiên chứa mật khẩu
        char storedPassLine[MAX_LEN];
        fgets(storedPassLine, sizeof(storedPassLine), fp);
        fclose(fp);

        char *storedPass = strchr(storedPassLine, ':');
        if (storedPass) {
            storedPass++; // bỏ qua dấu ':'
            storedPass[strcspn(storedPass, "\n")] = 0;
            caesarDecrypt(storedPass);
        }

        printf("Nhap mat khau: ");
        fgets(inputPass, sizeof(inputPass), stdin);
        inputPass[strcspn(inputPass, "\n")] = 0;

        if (strcmp(inputPass, storedPass) != 0) {
            printf("Mat khau sai!\n");
            return 1;
        }

        printf("Dang nhap thanh cong!\n");
    } else {
        printf("Lua chon khong hop le.\n");
        return 1;
    }

    // Sau khi đăng nhập
    printf("Ban muon them (1) hay xem (2) vi? Nhap 1 hoac 2: ");
    int choice;
    scanf("%d", &choice);
    getchar(); // loại bỏ ký tự '\n' sau scanf

    if (choice == 1) {
        char newUser[MAX_LEN], newPass[MAX_LEN];

        printf("Nhap username moi: ");
        fgets(newUser, sizeof(newUser), stdin);
        newUser[strcspn(newUser, "\n")] = 0;

        printf("Nhap password moi: ");
        fgets(newPass, sizeof(newPass), stdin);
        newPass[strcspn(newPass, "\n")] = 0;

        caesarEncrypt(newUser);
        caesarEncrypt(newPass);

        fp = fopen(filename, "a");
        if (!fp) {
            perror("Khong the mo file");
            return 1;
        }

        fprintf(fp, "USER:%s\n", newUser);
        fprintf(fp, "PASS:%s\n", newPass);
        fclose(fp);

        printf("Da luu thong tin thanh cong.\n");

    } else if (choice == 2) {
        fp = fopen(filename, "r");
        if (!fp) {
            perror("Khong the mo file");
            return 1;
        }

        char line[MAX_LEN];
        int isFirstLine = 1;

        while (fgets(line, sizeof(line), fp)) {
            if (isFirstLine) {
                isFirstLine = 0;
                continue; // bỏ qua dòng password
            }

            caesarDecrypt(line);
            printf("%s", line);
        }

        fclose(fp);
    } else {
        printf("Lua chon khong hop le.\n");
    }

    return 0;
}
