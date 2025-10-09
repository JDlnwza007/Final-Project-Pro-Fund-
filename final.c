#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// --- เพิ่ม Header และ Define ที่จำเป็น ---
#include "fort.h"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_GRAY   "\x1b[90m"
#define ANSI_COLOR_RESET  "\x1b[0m"
// -------------------------------------
#define FILENAME "shipping.csv"
#define MAX_LINE 1024
// ฟังก์ชันหลัก (เหมือนเดิม)
void read_data();
void add_data();
void save_data(const char *id, const char *name, const char *address, const char *status);
void edit_data();
void search_data();
void update_data();
void delete_data();
void display_menu();
extern void run_all_unit_tests();
extern void run_all_e2e_tests();
const char* get_colored_status(const char* status) {
// ใช้ static buffer เพื่อให้สามารถคืนค่า pointer ได้อย่างปลอดภัย
static char colored_status[100];
if (strcmp(status, "Delivered") == 0) {
    sprintf(colored_status, "%s%s%s", ANSI_COLOR_GREEN, status, ANSI_COLOR_RESET);
} else if (strcmp(status, "Shipped") == 0) {
    sprintf(colored_status, "%s%s%s", ANSI_COLOR_YELLOW, status, ANSI_COLOR_RESET);
} else if (strcmp(status, "Pending") == 0) {
    sprintf(colored_status, "%s%s%s", ANSI_COLOR_GRAY, status, ANSI_COLOR_RESET);
} else if (strcmp(status, "Deleted") == 0) {
    sprintf(colored_status, "%s%s%s", ANSI_COLOR_RED, status, ANSI_COLOR_RESET);
} else {
    strcpy(colored_status, status);
}
return colored_status;
}
// ----------- ฟังก์ชันที่ 1 อ่านข้อมูล CSV (เวอร์ชันแสดงสีตามสถานะ) ----------
void read_data() {
FILE *fp = fopen(FILENAME, "r");
if (fp == NULL) {
printf("\n--> ERROR: Cannot open file '%s'.\n", FILENAME);
return;
}
ft_table_t *table = ft_create_table();
ft_set_border_style(table, FT_BASIC_STYLE);

char line[MAX_LINE];
int is_header = 1;

while (fgets(line, sizeof(line), fp)) {
    line[strcspn(line, "\n\r")] = 0;
    if (strlen(line) == 0) continue;

    char buffer[MAX_LINE];
    strcpy(buffer, line);

    char *id = strtok(buffer, ",");
    char *name = strtok(NULL, ",");
    char *address = strtok(NULL, ",");
    char *status = strtok(NULL, "\n\r");

    if (id && name && address && status) {
        if (is_header) {
            ft_write_ln(table,
                        ANSI_COLOR_BLUE "DeliveryID" ANSI_COLOR_RESET,
                        ANSI_COLOR_BLUE "RecipientName" ANSI_COLOR_RESET,
                        ANSI_COLOR_BLUE "Address" ANSI_COLOR_RESET,
                        ANSI_COLOR_BLUE "DeliveryStatus" ANSI_COLOR_RESET);
            ft_add_separator(table);
            is_header = 0;
        } else {
            char colored_status[100];
            if (strcmp(status, "Delivered") == 0) {
                sprintf(colored_status, "%s%s%s", ANSI_COLOR_GREEN, status, ANSI_COLOR_RESET);
            } else if (strcmp(status, "Shipped") == 0) {
                sprintf(colored_status, "%s%s%s", ANSI_COLOR_YELLOW, status, ANSI_COLOR_RESET);
            } else if (strcmp(status, "Pending") == 0) {
                sprintf(colored_status, "%s%s%s", ANSI_COLOR_GRAY, status, ANSI_COLOR_RESET);
            } else if (strcmp(status, "Deleted") == 0) {
                sprintf(colored_status, "%s%s%s", ANSI_COLOR_RED, status, ANSI_COLOR_RESET);
            } else {
                strcpy(colored_status, status);
            }
            ft_write_ln(table, id, name, address, colored_status);
        }
    }
}
fclose(fp);
printf("\n===== Information from %s =====\n", FILENAME);
const char *table_string = ft_to_string(table);
if (table_string) {
    printf("%s\n", table_string);
}
ft_destroy_table(table);
printf("--- Read successfully ---\n");
}
// ---------- ฟังก์ชันที่ 2 เพิ่มข้อมูลใหม่ ----------
void add_data() {
char id[20], name[50], address[100], status[50];
char last_id[20] = "None";
char line[MAX_LINE];
int id_exists;
// --- Phase 1: แสดง ID ล่าสุด (ถ้ามี) ---
FILE *fp = fopen(FILENAME, "r");
if (fp != NULL) {
    if (fgets(line, sizeof(line), fp)) {
        while (fgets(line, sizeof(line), fp)) {
            sscanf(line, "%[^,],", last_id);
        }
    }
    fclose(fp);
    printf("\nThe last DeliveryID was %s\n", last_id);
}


// --- Phase 2: รับค่า ID และตรวจสอบการซ้ำซ้อน ---
do {
    id_exists = 0;
    printf("Enter DeliveryID (or enter '0' to cancel): ");
    scanf("%s", id);

    // ตรวจสอบการยกเลิก
    if (strcmp(id, "0") == 0) {
        printf("\n-- Add operation cancelled. Returning to menu. --\n");
        return;
    }
    fp = fopen(FILENAME, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            char existing_id[20];
            sscanf(line, "%[^,],", existing_id);
            if (strcmp(existing_id, id) == 0) {
                printf("-- ERROR: DeliveryID '%s' already exists. Please enter a new one. --\n", id);
                id_exists = 1;
                break;
            }
        }
        fclose(fp);
    }

} while (id_exists);


// --- Phase 3: รับข้อมูลส่วนที่เหลือ ---
printf("Enter RecipientName: ");
scanf(" %[^\n]", name);
printf("Enter Address: ");
scanf(" %[^\n]", address);

// --- Phase 4: เมนูเลือกสถานะ ---
int status_choice;
int valid_choice = 0;
do {
    printf("Select Delivery Status:\n");
    printf("  1. Pending\n");
    printf("  2. Shipped\n");
    printf("  3. Delivered\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &status_choice);

    switch (status_choice) {
        case 1: strcpy(status, "Pending"); valid_choice = 1; break;
        case 2: strcpy(status, "Shipped"); valid_choice = 1; break;
        case 3: strcpy(status, "Delivered"); valid_choice = 1; break;
        default:
            printf("\n-- Invalid choice. Please enter a number between 1 and 3. --\n");
    }
} while (!valid_choice);


// --- Phase 5: บันทึกข้อมูล ---
save_data(id, name, address, status);
printf("\n-- Record added successfully! --\n");
}
// ---------- บันทึกข้อมูลลงไฟล์ (เหมือนเดิม) ----------
void save_data(const char *id, const char *name, const char *address, const char *status) {
FILE *fp = fopen(FILENAME, "a");
if (fp == NULL) {
printf("-- Can not open %s this file to append! --\n", FILENAME);
return;
}
fprintf(fp, "%s,%s,%s,%s\n", id, name, address, status);
fclose(fp);
printf("\n-- Append successfully --\n");
}
// ---------- ฟังก์ชันที่ X: แก้ไขข้อมูล (เวอร์ชันแก้ไขต่อเนื่อง) ----------
void edit_data() {
char search_id[20];
char line[MAX_LINE];
char buffer[MAX_LINE];
int found = 0;
// --- Phase 1: ค้นหา ID ที่ต้องการแก้ไข ---
printf("Enter DeliveryID to edit (or enter '0' to cancel): ");
scanf("%s", search_id);

if (strcmp(search_id, "0") == 0) {
    printf("\n-- Edit operation cancelled. --\n");
    return;
}

// --- Phase 2: ตรวจสอบว่ามี ID นี้อยู่จริงหรือไม่ ---
FILE *fp = fopen(FILENAME, "r");
if (fp == NULL) { /* ... Error handling ... */ return; }
while (fgets(line, sizeof(line), fp)) {
    strcpy(buffer, line);
    char *id = strtok(buffer, ",");
    if (id && strcmp(id, search_id) == 0) {
        found = 1;
        break;
    }
}
fclose(fp);

if (!found) {
    printf("\n-- DeliveryID '%s' not found. --\n", search_id);
    return;
}

int keep_editing = 1;
do {
    // --- Phase 3: แสดงข้อมูลล่าสุดเสมอ ---
    char current_id[20], current_name[50], current_address[100], current_status[50];
    found = 0;
    fp = fopen(FILENAME, "r");
    while (fgets(line, sizeof(line), fp)) {
        strcpy(buffer, line);
        char *id = strtok(buffer, ",");
        if (id && strcmp(id, search_id) == 0) {
            char *name = strtok(NULL, ",");
            char *address = strtok(NULL, ",");
            char *status = strtok(NULL, "\n\r");
            if (name && address && status) {
                printf("\n--- Editing Record --- (Current/Updated Data)\n");
                printf("%-12s | %-25s | %-25s | %s\n", "DeliveryID", "RecipientName", "Address", "DeliveryStatus");
                printf("-------------|---------------------------|---------------------------|-----------------\n");
                printf("%-12s | %-25s | %-25s | %s\n", id, name, address, get_colored_status(status));
                strcpy(current_id, id); strcpy(current_name, name);
                strcpy(current_address, address); strcpy(current_status, status);
                found = 1;
            }
            break;
        }
    }
    fclose(fp);
    
    if (!found) { // กรณีฉุกเฉิน หากไฟล์มีปัญหา
        printf("\n-- Error: Could not re-read the record. Aborting. --\n");
        return;
    }

    // --- Phase 4: แสดงเมนูย่อยให้ผู้ใช้เลือก ---
    int edit_choice;
    printf("\nWhich field do you want to edit?\n");
    printf("  1. DeliveryID\n");
    printf("  2. RecipientName\n");
    printf("  3. Address\n");
    printf("  4. DeliveryStatus\n");
    printf("  5. Edit All Fields\n");
    printf("  0. Finish Editing\n");
    printf("Enter your choice: ");
    scanf("%d", &edit_choice);

    if (edit_choice == 0) {
        keep_editing = 0;
        continue;
    }
    
    // --- Phase 5: รับข้อมูลใหม่ตามที่ผู้ใช้เลือก ---
    char new_id[20], new_name[50], new_address[100], new_status[50];
    strcpy(new_id, current_id); strcpy(new_name, current_name);
    strcpy(new_address, current_address); strcpy(new_status, current_status);
    
    int id_exists;

    // แก้ไข DeliveryID
    if (edit_choice == 1 || edit_choice == 5) {
        do {
            id_exists = 0;
            printf("\nEnter New DeliveryID: ");
            scanf("%s", new_id);
            if (strcmp(new_id, current_id) != 0) {
                fp = fopen(FILENAME, "r");
                while (fgets(line, sizeof(line), fp)) {
                    sscanf(line, "%[^,],", buffer);
                    if (strcmp(buffer, new_id) == 0) {
                        printf("-- ERROR: New DeliveryID '%s' already exists. --\n", new_id);
                        id_exists = 1;
                        break;
                    }
                }
                fclose(fp);
            }
        } while (id_exists);
    }

    // แก้ไข RecipientName
    if (edit_choice == 2 || edit_choice == 5) {
        printf("Enter New RecipientName: ");
        scanf(" %[^\n]", new_name);
    }

    // แก้ไข Address
    if (edit_choice == 3 || edit_choice == 5) {
        printf("Enter New Address: ");
        scanf(" %[^\n]", new_address);
    }

    // แก้ไข DeliveryStatus
    if (edit_choice == 4 || edit_choice == 5) {
        int status_choice, valid_choice = 0;
        do {
            printf("\nSelect New Delivery Status:\n");
            printf("  1. Pending\n");
            printf("  2. Shipped\n");
            printf("  3. Delivered\n");
            printf("Enter your choice (1-3): ");
            scanf("%d", &status_choice);
            switch (status_choice) {
                case 1: strcpy(new_status, "Pending"); valid_choice = 1; break;
                case 2: strcpy(new_status, "Shipped"); valid_choice = 1; break;
                case 3: strcpy(new_status, "Delivered"); valid_choice = 1; break;
                default: printf("\n-- Invalid choice. --\n");
            }
        } while (!valid_choice);
    }

    // --- Phase 6: เขียนข้อมูลลงไฟล์ใหม่ ---
    fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.csv", "w");
    if (fp == NULL || temp == NULL) { /* ... */ return; }
    
    while (fgets(line, sizeof(line), fp)) {
        strcpy(buffer, line);
        char *id_check = strtok(buffer, ",");
        if (id_check && strcmp(id_check, current_id) == 0) {
            fprintf(temp, "%s,%s,%s,%s\n", new_id, new_name, new_address, new_status);
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(fp);
    fclose(temp);
    
    remove(FILENAME);
    rename("temp.csv", FILENAME);

    printf("\n-- Record updated successfully! --\n");
    strcpy(search_id, new_id);
    if (edit_choice == 5) {
        keep_editing = 0;
    }

} while (keep_editing);

printf("\n-- Finished editing. Returning to main menu. --\n");
}
// ---------- ฟังก์ชันที่ 4 ค้นหาข้อมูล (เวอร์ชันแสดงผลสวยงามและยกเลิกได้) ----------
void search_data() {
char keyword[50];
char line[MAX_LINE];
int found = 0;
int is_header = 1;
printf("Enter DeliveryID or RecipientName to search (or enter '0' to cancel): ");
scanf(" %[^\n]", keyword);

// ตรวจสอบการยกเลิก
if (strcmp(keyword, "0") == 0) {
    printf("\n-- Search operation cancelled. Returning to menu. --\n");
    return;
}

FILE *fp = fopen(FILENAME, "r");
if (fp == NULL) {
    printf("-- Cannot open this file! --\n");
    return;
}

printf("\n--- Search Results for '%s' ---\n", keyword);

if (fgets(line, sizeof(line), fp) == NULL) {
    fclose(fp);
    printf("File is empty.\n");
    printf("--- End of Search ---\n");
    return;
}
while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, keyword)) {
        if (is_header) {
            printf("\n%-12s | %-25s | %-25s | %-15s\n", "DeliveryID", "RecipientName", "Address", "DeliveryStatus");
            printf("-------------|---------------------------|---------------------------|-----------------\n");
            is_header = 0;
        }
        char buffer[MAX_LINE];
        strcpy(buffer, line);
        line[strcspn(line, "\n\r")] = 0;

        char *id = strtok(buffer, ",");
        char *name = strtok(NULL, ",");
        char *address = strtok(NULL, ",");
        char *status = strtok(NULL, "\n\r");
        if (id && name && address && status) {
            printf("%-12s | %-25s | %-25s | %-15s\n", id, name, address, get_colored_status(status));
        } else {
            printf("Found malformed data: %s\n", line);
        }
        found = 1;
    }
}

if (!found) {
    printf("No information found.\n");
}
printf("\n--- End of Search ---\n");

fclose(fp);
}
// ---------- ฟังก์ชันที่ 5 อัปเดตข้อมูล (เวอร์ชันเลือกสถานะจากเมนู) ----------
void update_data() {
char keyword[100];
char line[MAX_LINE];
char buffer[MAX_LINE];
int found = 0;
// --- Phase 1: Search (เหมือนเดิม) ---
printf("Enter DeliveryID or Part of Name to update (or enter '0' to cancel): ");
scanf(" %[^\n]", keyword);

if (strcmp(keyword, "0") == 0) {
    printf("\n-- Update operation cancelled. --\n");
    return;
}

FILE *fp = fopen(FILENAME, "r");
if (fp == NULL) {
    printf("-- Cannot open file! --\n");
    return;
}

// ---- Phase 2: Find and Display (เหมือนเดิม) ----
char found_id[20];

fgets(line, sizeof(line), fp);

while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, keyword)) {
        printf("\n--- Record Found ---\n");
        printf("%-12s | %-25s | %-25s | %-15s\n", "DeliveryID", "RecipientName", "Address", "DeliveryStatus");
        printf("-------------|---------------------------|---------------------------|-----------------\n");
        
        strcpy(buffer, line);
        char *id = strtok(buffer, ",");
        char *name = strtok(NULL, ",");
        char *address = strtok(NULL, ",");
        char *status = strtok(NULL, "\n\r");

        if (id && name && address && status) {
             printf("%-12s | %-25s | %-25s | %-15s\n", id, name, address, get_colored_status(status));
             strcpy(found_id, id);
             found = 1;
        }
        break;
    }
}
fclose(fp);

// ---- Phase 3: Get New Data and Act ----
if (!found) {
    printf("\n-- Record '%s' not found. --\n", keyword);
    return;
}
int status_choice;
char new_status[50];
int valid_choice = 0;
do {
    printf("\nSelect New Status for this record:\n");
    printf("  1. Pending\n");
    printf("  2. Shipped\n");
    printf("  3. Delivered\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &status_choice);

    switch (status_choice) {
        case 1:
            strcpy(new_status, "Pending");
            valid_choice = 1;
            break;
        case 2:
            strcpy(new_status, "Shipped");
            valid_choice = 1;
            break;
        case 3:
            strcpy(new_status, "Delivered");
            valid_choice = 1;
            break;
        default:
            printf("\n-- Invalid choice. Please enter a number between 1 and 3. --\n");
    }
} while (!valid_choice);

fp = fopen(FILENAME, "r");
FILE *temp = fopen("temp.csv", "w");
if (fp == NULL || temp == NULL) {
    printf("-- Error creating temp file! --\n");
    return;
}

fprintf(temp, "DeliveryID,RecipientName,Address,DeliveryStatus\n");
fgets(line, sizeof(line), fp); 
while (fgets(line, sizeof(line), fp)) {
    strcpy(buffer, line);
    char *id_check = strtok(buffer, ",");

    if (id_check && strcmp(id_check, found_id) == 0) {
        char *name = strtok(NULL, ",");
        char *address = strtok(NULL, ",");
        fprintf(temp, "%s,%s,%s,%s\n", found_id, name, address, new_status);
    } else {
        fprintf(temp, "%s", line);
    }
}
fclose(fp);
fclose(temp);
remove(FILENAME);
rename("temp.csv", FILENAME);

printf("-- Update Successfully --\n");
}
// ---------- ฟังก์ชันที่ 6 ลบข้อมูล (เวอร์ชันแก้ไข Bug การค้นหา) ----------
void delete_data() {
char keyword[100];
char line[MAX_LINE];
char buffer[MAX_LINE];
int found = 0;
// ---- Phase 1: Search ----
printf("Enter DeliveryID or Part of Name to mark as 'Deleted' (or enter '0' to cancel): ");
scanf(" %[^\n]", keyword);

if (strcmp(keyword, "0") == 0) {
    printf("\n-- Mark operation cancelled. --\n");
    return;
}

FILE *fp = fopen(FILENAME, "r");
if (fp == NULL) {
    printf("-- Cannot open file! --\n");
    return;
}

// --- Phase 2: Find and Display ---
char found_id[20];
fgets(line, sizeof(line), fp);
while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, keyword)) {
        printf("\n--- Record Found ---\n");
        printf("%-12s | %-25s | %-25s | %-15s\n", "DeliveryID", "RecipientName", "Address", "DeliveryStatus");
        printf("-------------|---------------------------|---------------------------|-----------------\n");
        
        strcpy(buffer, line);
        char *id = strtok(buffer, ",");
        char *name = strtok(NULL, ",");
        char *address = strtok(NULL, ",");
        char *status = strtok(NULL, "\n\r");
        
        if (id && name && address && status) {
            printf("%-12s | %-25s | %-25s | %-15s\n", id, name, address, get_colored_status(status));
            strcpy(found_id, id);
            found = 1;
        }
        break;
    }
}
fclose(fp);

// --- Phase 3: Confirm and Act ---
if (!found) {
    printf("\n-- Record '%s' not found. --\n", keyword);
    return;
}

char confirm;
printf("\nAre you sure you want to mark this record as 'Deleted'? (y/n): ");
scanf(" %c", &confirm);

if (confirm != 'y' && confirm != 'Y') {
    printf("-- Mark operation cancelled by user. --\n");
    return;
}

fp = fopen(FILENAME, "r");
FILE *temp = fopen("temp.csv", "w");
if (fp == NULL || temp == NULL) {
    printf("-- Error creating temp file! --\n");
    return;
}

fprintf(temp, "DeliveryID,RecipientName,Address,DeliveryStatus\n");
fgets(line, sizeof(line), fp);
while (fgets(line, sizeof(line), fp)) {
    strcpy(buffer, line);
    char *id_check = strtok(buffer, ",");

    if (id_check && strcmp(id_check, found_id) == 0) {
        char *name = strtok(NULL, ",");
        char *address = strtok(NULL, ",");
        fprintf(temp, "%s,%s,%s,Deleted\n", found_id, name, address);
    } else {
        fprintf(temp, "%s", line);
    }
}
fclose(fp);
fclose(temp);
remove(FILENAME);
rename("temp.csv", FILENAME);

printf("-- Marked Successfully --\n");
}
// ---------- ฟังก์ชันที่ 7 แสดงเมนู (เหมือนเดิม) ----------
void display_menu() {
    int choice;
    do {
        printf("\n===== SHIPPING MANAGEMENT SYSTEM =====\n");
        printf("1. READ DATA\n");
        printf("2. ADD DATA\n");
        printf("3. EDIT DATA\n");
        printf("4. SEARCH DATA\n");
        printf("5. UPDATE STATUS\n");
        printf("6. MARK DATA AS DELETED\n");
        printf("7. EXIT THE SYSTEM\n");
        printf("--------------------------------------\n");
        printf(ANSI_COLOR_YELLOW "8. RUN ALL UNIT TESTS\n" ANSI_COLOR_RESET);
        printf(ANSI_COLOR_YELLOW "9. RUN E2E SCENARIO TEST\n" ANSI_COLOR_RESET);
        printf("--------------------------------------\n");
        printf("MENU: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: read_data(); break;
            case 2: add_data(); break;
            case 3: edit_data(); break;
            case 4: search_data(); break;
            case 5: update_data(); break;
            case 6: delete_data(); break;
            case 7: printf("-- Exiting the system --\n"); break;
            case 8:
                run_all_unit_tests();
                break;
            case 9:
                run_all_e2e_tests();
                break;
            default: printf("-- Invalid menu choice --\n");
        }
    } while (choice != 7);
}
// ---------- main (เวอร์ชันปรับปรุง) ----------
int main() {
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        fp = fopen(FILENAME, "w");
        if (fp != NULL) {
            fprintf(fp, "DeliveryID,RecipientName,Address,DeliveryStatus\n");
            fclose(fp);
            printf("Notice: '%s' was not found. A new, empty file has been created.\n", FILENAME);
        } else {
            printf("FATAL ERROR: Could not create '%s'. Please check permissions.\n", FILENAME);
            return 1;
        }
    } else {
        fclose(fp);
    }
    
    display_menu();
    return 0;
}