// =================================================================
// File: unit_test.c (เวอร์ชันแสดงผลลัพธ์แบบสรุป)
// =================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

// --- Extern functions from other files ---
extern void add_data();
extern void edit_data();
extern void update_data();
extern void delete_data();
extern void search_data();

// --- Test-specific function prototypes ---
void mock_stdin(const char* input);
void restore_stdin();

#define ORIGINAL_FILENAME "shipping.csv"
#define STDOUT_REDIRECT_FILENAME "test_output.log"

// --- Helper Functions for Testing (ไม่มีการเปลี่ยนแปลง) ---
void create_clean_test_file() {
    remove(ORIGINAL_FILENAME);
    FILE *fp = fopen(ORIGINAL_FILENAME, "w");
    assert(fp != NULL);
    fprintf(fp, "DeliveryID,RecipientName,Address,DeliveryStatus\n");
    fprintf(fp, "D001,Somchai Jaidee,123 Bangkok Thailand,Delivered\n");
    fprintf(fp, "D002,John Doe,456 California USA,Shipped\n");
    fprintf(fp, "D003,Jane Smith,789 London UK,Pending\n");
    fclose(fp);
}
bool file_contains_string(const char* filename, const char* str) {
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
void mock_stdin(const char* input) {
    FILE* in_file = fopen("mock_input.tmp", "w");
    assert(in_file != NULL);
    fputs(input, in_file);
    fclose(in_file);
    freopen("mock_input.tmp", "r", stdin);
}
void restore_stdin() {
    freopen("CON", "r", stdin);
    remove("mock_input.tmp");
}
void redirect_stdout() {
    freopen(STDOUT_REDIRECT_FILENAME, "w", stdout);
}
void restore_stdout() {
    freopen("CON", "w", stdout);
}


// --- Test Cases (เวอร์ชันปรับปรุงการแสดงผล) ---

// ====== ADD_DATA TESTS ======
void test_add_data_positive_normal() {
    printf("  %-70s", "[add_data] Positive: Should add a new unique record");
    create_clean_test_file();
    mock_stdin("D004\nNew User\n4 New Rd\n2\n");
    
    redirect_stdout(); // <<< ซ่อน Output
    add_data();
    restore_stdout();  // <<< เรียกคืน Output
    
    restore_stdin();
    assert(file_contains_string(ORIGINAL_FILENAME, "D004,New User,4 New Rd,Shipped"));
    printf("[PASS]\n");
}

void test_add_data_negative_duplicate_id() {
    printf("  %-70s", "[add_data] Negative: Should prevent adding a duplicate ID");
    create_clean_test_file();
    mock_stdin("D001\n0\n");

    redirect_stdout();
    add_data();
    restore_stdout();

    restore_stdin();
    assert(!file_contains_string(ORIGINAL_FILENAME, "D001,Cancelled User"));
    printf("[PASS]\n");
}

// ====== EDIT_DATA TESTS ======
void test_edit_data_positive_normal() {
    printf("  %-70s", "[edit_data] Positive: Should edit an existing record");
    create_clean_test_file();
    mock_stdin("D002\n2\nJohn Doe Updated\n0\n");

    redirect_stdout();
    edit_data();
    restore_stdout();

    restore_stdin();
    assert(file_contains_string(ORIGINAL_FILENAME, "D002,John Doe Updated,456 California USA,Shipped"));
    printf("[PASS]\n");
}

void test_edit_data_negative_id_not_found() {
    printf("  %-70s", "[edit_data] Negative: Should handle non-existent ID");
    create_clean_test_file();
    mock_stdin("D999\n");

    redirect_stdout();
    edit_data();
    restore_stdout();
    
    restore_stdin();
    assert(file_contains_string(ORIGINAL_FILENAME, "D003,Jane Smith,789 London UK,Pending"));
    printf("[PASS]\n");
}

// ====== UPDATE_DATA TESTS ======
void test_update_data_positive_normal() {
    printf("  %-70s", "[update_data] Positive: Should update status of a record");
    create_clean_test_file();
    mock_stdin("D003\n3\n");

    redirect_stdout();
    update_data();
    restore_stdout();

    restore_stdin();
    assert(file_contains_string(ORIGINAL_FILENAME, "D003,Jane Smith,789 London UK,Delivered"));
    printf("[PASS]\n");
}

// ====== DELETE_DATA TESTS ======
void test_delete_data_positive_confirm() {
    printf("  %-70s", "[delete_data] Positive: Should mark record as 'Deleted' on confirm");
    create_clean_test_file();
    mock_stdin("D002\ny\n");

    redirect_stdout();
    delete_data();
    restore_stdout();

    restore_stdin();
    assert(file_contains_string(ORIGINAL_FILENAME, "D002,John Doe,456 California USA,Deleted"));
    printf("[PASS]\n");
}

void test_delete_data_negative_cancel() {
    printf("  %-70s", "[delete_data] Negative: Should not change record on cancel");
    create_clean_test_file();
    mock_stdin("D002\nn\n");

    redirect_stdout();
    delete_data();
    restore_stdout();
    
    restore_stdin();
    assert(file_contains_string(ORIGINAL_FILENAME, "D002,John Doe,456 California USA,Shipped"));
    printf("[PASS]\n");
}

// ====== SEARCH_DATA TESTS ======
void test_search_data_positive_found() {
    printf("  %-70s", "[search_data] Positive: Should find and display existing data");
    create_clean_test_file();
    mock_stdin("Jane\n");
    
    redirect_stdout(); // เราใช้ redirect อยู่แล้วสำหรับ search_data
    search_data();
    restore_stdout();
    
    restore_stdin();
    assert(file_contains_string(STDOUT_REDIRECT_FILENAME, "Jane Smith"));
    printf("[PASS]\n");
}

void test_search_data_negative_not_found() {
    printf("  %-70s", "[search_data] Negative: Should show message when data not found");
    create_clean_test_file();
    mock_stdin("NonExistentUser\n");
    
    redirect_stdout();
    search_data();
    restore_stdout();
    
    restore_stdin();
    assert(file_contains_string(STDOUT_REDIRECT_FILENAME, "No information found"));
    printf("[PASS]\n");
}


// --- Main Test Runner ---
void run_all_unit_tests() {
    printf("\n--- Running Comprehensive Unit Tests ---\n");
    
    FILE *backup_check = fopen(ORIGINAL_FILENAME, "r");
    bool backup_created = false;
    if (backup_check) {
        fclose(backup_check);
        rename(ORIGINAL_FILENAME, "shipping.csv.bak");
        backup_created = true;
    }

    // --- รัน Test Cases ทั้งหมด ---
    printf("====================== ADD_DATA ==========================================\n");
    test_add_data_positive_normal();
    test_add_data_negative_duplicate_id();

    printf("====================== EDIT_DATA =========================================\n");
    test_edit_data_positive_normal();
    test_edit_data_negative_id_not_found();

    printf("====================== UPDATE_DATA =======================================\n");
    test_update_data_positive_normal();

    printf("====================== DELETE_DATA =======================================\n");
    test_delete_data_positive_confirm();
    test_delete_data_negative_cancel();

    printf("====================== SEARCH_DATA =======================================\n");
    test_search_data_positive_found();
    test_search_data_negative_not_found();
    
    // --- คืนค่าไฟล์เดิม ---
    remove(ORIGINAL_FILENAME);
    if (backup_created) {
        rename("shipping.csv.bak", ORIGINAL_FILENAME);
    }
    remove(STDOUT_REDIRECT_FILENAME);
    remove("mock_input.tmp");

    printf("==========================================================================\n");
    printf("\n--- All Unit Tests Finished Successfully ---\n\n");
}