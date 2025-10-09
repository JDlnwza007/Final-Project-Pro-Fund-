// =================================================================
// File: E2E_test.c (เวอร์ชันปรับปรุงการแสดงผล)
// =================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

// --- Extern functions from prototype_04.c ---
extern void add_data();
extern void edit_data();
extern void update_data();
extern void delete_data();
extern void read_data();

// --- Extern functions from unit_test.c ---
extern void mock_stdin(const char* input);
extern void restore_stdin();

#define FILENAME "shipping.csv"

// --- Helper Functions for E2E Test ---

// สร้างสภาพแวดล้อมที่สะอาดสำหรับ E2E test
void setup_e2e_test_environment() {
    FILE* check = fopen(FILENAME, "r");
    if (check) {
        fclose(check);
        rename(FILENAME, "shipping.csv.e2e.bak");
    }
    FILE *fp = fopen(FILENAME, "w");
    assert(fp != NULL);
    fprintf(fp, "DeliveryID,RecipientName,Address,DeliveryStatus\n");
    fprintf(fp, "BASE01,Base User,Base Address,Pending\n");
    fclose(fp);
}

// คืนค่าสภาพแวดล้อมเดิม
void cleanup_e2e_test_environment() {
    remove(FILENAME);
    FILE* check = fopen("shipping.csv.e2e.bak", "r");
    if (check) {
        fclose(check);
        rename("shipping.csv.e2e.bak", FILENAME);
    }
}

// Helper function to check if a file contains a string
bool file_contains_string_e2e(const char* filename, const char* str) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return false;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, str)) {
            fclose(fp);
            return true;
        }
    }
    fclose(fp);
    return false;
}

// --- E2E Test Scenario (เวอร์ชันแก้ไข) ---

void test_full_user_journey() {
    // พิมพ์หัวข้อเทสต์และรอผลลัพธ์ในบรรทัดเดียวกัน
    printf("  [TEST] E2E Journey: Should complete the full add-update-edit-delete cycle... ");

    // Step 1: ADD
    mock_stdin("E2E-001\nJourney User\n123 E2E Lane\n1\n");
    add_data();
    restore_stdin();
    assert(file_contains_string_e2e(FILENAME, "E2E-001,Journey User,123 E2E Lane,Pending"));

    // Step 2: UPDATE
    mock_stdin("E2E-001\n2\n");
    update_data();
    restore_stdin();
    assert(file_contains_string_e2e(FILENAME, "E2E-001,Journey User,123 E2E Lane,Shipped"));

    // Step 3: EDIT
    mock_stdin("E2E-001\n3\n456 E2E Boulevard\n0\n");
    edit_data();
    restore_stdin();
    assert(file_contains_string_e2e(FILENAME, "E2E-001,Journey User,456 E2E Boulevard,Shipped"));

    // Step 4: DELETE
    mock_stdin("E2E-001\ny\n");
    delete_data();
    restore_stdin();
    assert(file_contains_string_e2e(FILENAME, "E2E-001,Journey User,456 E2E Boulevard,Deleted"));

    // ถ้าทุก assert ผ่านหมด ให้พิมพ์ PASS เพื่อจบการทดสอบ
    printf("PASS\n");
}


// --- Main E2E runner function (เวอร์ชันแก้ไข) ---
void run_all_e2e_tests() {
    printf("\n--- Running End-to-End (E2E) Tests ---\n");

    setup_e2e_test_environment();

    printf("\nTesting Scenario: Full User Journey\n");
    test_full_user_journey();
    // คุณสามารถเพิ่ม E2E Scenarios อื่นๆ ที่นี่ได้ในอนาคต

    cleanup_e2e_test_environment();

    printf("\n--- All E2E Tests Finished ---\n\n");
}