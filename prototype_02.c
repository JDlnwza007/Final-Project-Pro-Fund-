#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// --- เพิ่ม Header และ Define ที่จำเป็น ---
#include "fort.h"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_RESET  "\x1b[0m"
#define FILENAME "shipping.csv"
#define MAX_LINE 1024

void read_data();
void add_data();
void save_data(const char *id, const char *name, const char *address, const char *status);
void search_data();
void update_data();
void delete_data();
void display_menu();


// =================================================================================
// ---------- ฟังก์ชันที่ 1 อ่านข้อมูล CSV
// =================================================================================
void read_data() {
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf("\n--> ERROR: Cannot open file '%s'. Please add data first.\n", FILENAME);
        return;
    }
    ft_table_t *table = ft_create_table();
    ft_set_border_style(table, FT_BASIC_STYLE); 

    char line[MAX_LINE];
    int is_header = 1; 

   
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n\r")] = 0;
        if (strlen(line) == 0) {
            continue;
        }

        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");
        char *address = strtok(NULL, ",");
        char *status = strtok(NULL, ",");

        if (id && name && address && status) {
            if (is_header) {
                ft_write_ln(table,
                            ANSI_COLOR_BLUE "DeliveryID" ANSI_COLOR_RESET,
                            ANSI_COLOR_BLUE "RecipientName" ANSI_COLOR_RESET,
                            ANSI_COLOR_BLUE "Address" ANSI_COLOR_RESET,
                            ANSI_COLOR_BLUE "DeliveryStatus" ANSI_COLOR_RESET);
                ft_add_separator(table); 
                is_header = 0; 
            } 
            else 
            {
                ft_write_ln(table, id, name, address, status);
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


// ---------- ฟังก์ชันที่ 2 เพิ่มข้อมูลใหม่ (เวอร์ชันยกเลิกได้) ----------
void add_data() {
    char id[20], name[50], address[100], status[50];
    printf("Enter DeliveryID (or enter '0' to cancel): ");
    scanf("%s", id);

    if (strcmp(id, "0") == 0) {
        printf("\n-- Add operation cancelled. Returning to menu. --\n");
        return;
    }

    printf("Enter RecipientName: ");
    scanf(" %[^\n]", name);
    printf("Enter Address: ");
    scanf(" %[^\n]", address);
    printf("Enter DeliveryStatus: ");
    scanf("%s", status);

    save_data(id, name, address, status);
    printf("-- Add succesfully --\n");
}

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

void search_data() {
    char keyword[50];
    char line[MAX_LINE];
    int found = 0;
    int is_header = 1;

    printf("Enter DeliveryID or RecipientName to search (or enter '0' to cancel): ");
    scanf(" %[^\n]", keyword);

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
                printf("%-12s | %-25s | %-25s | %-15s\n", id, name, address, status);
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


// ---------- ฟังก์ชันที่ 5 อัปเดตข้อมูล (เวอร์ชันแก้ไข Bug การค้นหา) ----------
void update_data() {
    char keyword[100];
    char line[MAX_LINE];
    char buffer[MAX_LINE];
    int found = 0;
    
    // --- Phase 1: Search ---
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
                 printf("%-12s | %-25s | %-25s | %-15s\n", id, name, address, status);
                 strcpy(found_id, id); 
                 found = 1;
            }
            break;
        }
    }
    fclose(fp);

    // --- Phase 3: Get New Data and Act ---
    if (!found) {
        printf("\n-- Record '%s' not found. --\n", keyword);
        return;
    }

    char new_status[50];
    printf("\nEnter New Status for this record: ");
    scanf("%s", new_status);

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

    // --- Phase 1: Search ---
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
                printf("%-12s | %-25s | %-25s | %-15s\n", id, name, address, status);
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
            default: printf("-- No menu that you selected --\n");
        }
    } while (choice != 7);
}


// ---------- main (เหมือนเดิม) ----------
int main() {
    display_menu();
    return 0;
}