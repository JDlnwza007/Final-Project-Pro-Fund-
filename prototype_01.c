#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define FILENAME "shipping.csv"
#define MAX_LINE 10241

// ฟังก์ชันหลัก
void read_data();
void add_data();
void save_data(const char *id, const char *name, const char *address, const char *status);
void search_data();
void update_data();
void delete_data();
void display_menu();


// ---------- ฟังก์ชันที่ 1 อ่านข้อมูล CSV ----------
void read_data() {
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf("-- Can not open %s this --\n", FILENAME);
        return;
    }

    char line[MAX_LINE];
    printf("\n Information:\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
    printf("\n-- Read successfully --\n");
}


// ---------- ฟังก์ชันที่ 2 เพิ่มข้อมูลใหม่ ----------
void add_data() {
    char id[20], name[50], address[100], status[50];

    printf("Enter DeliveryID: ");
    scanf("%s", id);
    printf("Enter RecipientName: ");
    scanf(" %[^\n]", name);   // อ่านทั้งบรรทัด
    printf("Enter Address: ");
    scanf(" %[^\n]", address);
    printf("Enter DeliveryStatus: ");
    scanf("%s", status);

    save_data(id, name, address, status);
    printf("-- Add succesfully --\n");
}


// ---------- ฟังก์ชันที่ 3 บันทึกข้อมูลลงไฟล์ ----------
void save_data(const char *id, const char *name, const char *address, const char *status) {
    FILE *fp = fopen(FILENAME, "a");  // append
    if (fp == NULL) {
        printf("-- Can not open %s this file to append! --\n", FILENAME);
        return;
    }

    fprintf(fp, "%s,%s,%s,%s\n", id, name, address, status);
    fclose(fp);
    printf("\n-- Append successfully --\n");
}


// ---------- ฟังก์ชันที่ 4 ค้นหาข้อมูล ----------
void search_data() {
    char keyword[50];
    char line[MAX_LINE];
    int found = 0;

    printf(" Enter your DeliveryID or RecipientName: ");
    scanf(" %[^\n]", keyword);

    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf("-- Can not open this file! --\n");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, keyword)) {
            printf("This is your Status: %s", line);
            found = 1;
        }
    }

    if (!found) {
        printf(" Not found Information\n");
    }

    fclose(fp);
}


// ---------- ฟังก์ชันที่ 5 อัพเดทข้อมูล ----------
void update_data() {
    char id[20], new_status[50];
    printf("Enter DeliveryID (Update): ");
    scanf("%s", id);
    printf("Enter New Status: ");
    scanf("%s", new_status);

    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.csv", "w");

    if (fp == NULL || temp == NULL) {
        printf("-- Can not open this file! --\n");
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        char cid[20], name[50], address[100], status[50];
        sscanf(line, "%[^,],%[^,],%[^,],%s", cid, name, address, status);

        if (strcmp(cid, id) == 0) {
            fprintf(temp, "%s,%s,%s,%s\n", cid, name, address, new_status);
            printf("-- Update Succesfully --\n");
        } else {
            fprintf(temp, "%s", line);
        }
    }

    fclose(fp);
    fclose(temp);
    remove(FILENAME);
    rename("temp.csv", FILENAME);
}


// ---------- ฟังก์ชันที่ 6 ลบข้อมูล (mark delete) ----------
void delete_data() {
    char id[20];
    printf("Enter DeliveryID to (mark): ");
    scanf("%s", id);

    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.csv", "w");

    if (fp == NULL || temp == NULL) {
        printf("-- Can not open this file! --\n");
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        char cid[20], name[50], address[100], status[50];
        sscanf(line, "%[^,],%[^,],%[^,],%s", cid, name, address, status);

        if (strcmp(cid, id) == 0) {
            fprintf(temp, "%s,%s,%s,Deleted\n", cid, name, address);
            printf("-- Mark Successfully --\n");
        } else {
            fprintf(temp, "%s", line);
        }
    }

    fclose(fp);
    fclose(temp);
    remove(FILENAME);
    rename("temp.csv", FILENAME);
}


// ---------- ฟังก์ชันที่ 7 แสดงเมนู ----------
void display_menu() {
    int choice;
    do {
        printf("\n===== SHIPPING MANAGEMENT SYSTEM =====\n");
        printf("1. READ DATA\n");
        printf("2. ADD DATA\n");
        printf("3. APPEND DATA\n");
        printf("4. SEARCH DATA\n");
        printf("5. UPDATE STATUS\n");
        printf("6. MARK DATA\n");
        printf("7. EXIT THE SYSTEM\n");
        printf("MENU: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: read_data(); break;
            case 2: add_data(); break;
            case 3: printf("append in add_data automatic\n"); break;
            case 4: search_data(); break;
            case 5: update_data(); break;
            case 6: delete_data(); break;
            case 7: printf("--exit the system --\n"); break;
            default: printf("--No menu that you selected --\n");
        }
    } while (choice != 7);
}


// ---------- main ----------
int main() {
    display_menu();
    return 0;
}

