#include "../include/file_io.h"
#include "../include/table.h"
#include "../include/record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"../include/cJSON.h"
#include"../include/libxlsxwriter.h"

// CSV: Save table to a CSV file
void save_to_csv(const char* filename, Table* table) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for saving");
        return;
    }

    // Write column headers
    for (int i = 0; i < table->num_columns; i++) {
        fprintf(file, "%s", table->columns[i].name);
        if (i < table->num_columns - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    // Write records
    Record* current = table->head;
    while (current != NULL) {
        for (int i = 0; i < table->num_columns; i++) {
            if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                fprintf(file, "%d", *(int*)current->data[i]);
            } else if (strcmp(table->columns[i].type, "STRING") == 0) {
                fprintf(file, "%s", (char*)current->data[i]);
            }
            if (i < table->num_columns - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
        current = current->next;
    }

    fclose(file);
    printf("Table '%s' saved to CSV file '%s'.\n", table->name, filename);
}

// CSV: Load table from a CSV file
void load_from_csv(const char* filename, Table* table) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for loading");
        return;
    }

    char line[1024];
    fgets(line, sizeof(line), file); // Skip header line

    while (fgets(line, sizeof(line), file)) {
        char* token;
        char* rest = line;
        char** values = (char**)malloc(table->num_columns * sizeof(char*));  // Dynamically allocate memory
        if (!values) {
            perror("Memory allocation failed");
            fclose(file);
            return;
        }

        int i = 0;
        while ((token = strtok_r(rest, ",\n", &rest))) {
            values[i++] = token;
        }

        // Create and add record
        Record* new_record = create_record(table, values);
        if (new_record == NULL) {
            printf("Error: Failed to create record.\n");
            free(values);  // Free allocated memory
            continue;
        }

        if (table->head == NULL) {
            table->head = new_record;
            table->tail = new_record;
        } else {
            new_record->next = table->head;
            table->head->prev = new_record;
            table->head = new_record;
        }

        free(values);  // Free allocated memory
    }

    fclose(file);
    printf("Table '%s' loaded from CSV file '%s'.\n", table->name, filename);
}

// JSON: Save table to a JSON file
void save_to_json(const char* filename, Table* table) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for saving");
        return;
    }

    fprintf(file, "[\n");

    Record* current = table->head;
    while (current != NULL) {
        fprintf(file, "  {\n");
        for (int i = 0; i < table->num_columns; i++) {
            fprintf(file, "    \"%s\": ", table->columns[i].name);
            if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                fprintf(file, "%d", *(int*)current->data[i]);
            } else if (strcmp(table->columns[i].type, "STRING") == 0) {
                fprintf(file, "\"%s\"", (char*)current->data[i]);
            }
            if (i < table->num_columns - 1) {
                fprintf(file, ",\n");
            } else {
                fprintf(file, "\n");
            }
        }
        fprintf(file, "  }");
        if (current->next != NULL) {
            fprintf(file, ",\n");
        } else {
            fprintf(file, "\n");
        }
        current = current->next;
    }

    fprintf(file, "]\n");
    fclose(file);
    printf("Table '%s' saved to JSON file '%s'.\n", table->name, filename);
}

// JSON: Load table from a JSON file
void load_from_json(const char* filename, Table* table) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for loading");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* json_data = (char*)malloc(file_size + 1);
    fread(json_data, 1, file_size, file);
    json_data[file_size] = '\0';
    fclose(file);

    // Use cJSON library to parse JSON
    #include "cJSON.h"
    cJSON* json = cJSON_Parse(json_data);
    if (!json) {
        printf("Error: Failed to parse JSON.\n");
        free(json_data);
        return;
    }

    cJSON* record;
    cJSON_ArrayForEach(record, json) {
        char** values = (char**)malloc(table->num_columns * sizeof(char*));  // Dynamically allocate memory
        if (!values) {
            perror("Memory allocation failed");
            cJSON_Delete(json);
            free(json_data);
            return;
        }

        for (int i = 0; i < table->num_columns; i++) {
            cJSON* item = cJSON_GetObjectItemCaseSensitive(record, table->columns[i].name);
            if (item) {
                if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                    values[i] = (char*)malloc(20);
                    sprintf(values[i], "%d", item->valueint);
                } else if (strcmp(table->columns[i].type, "STRING") == 0) {
                    values[i] = _strdup(item->valuestring);  // Use _strdup for MSVC compatibility
                }
            } else {
                values[i] = _strdup("");  // Use _strdup for MSVC compatibility
            }
        }

        Record* new_record = create_record(table, values);
        if (new_record == NULL) {
            printf("Error: Failed to create record.\n");
            for (int i = 0; i < table->num_columns; i++) {
                free(values[i]);
            }
            free(values);
            continue;
        }

        if (table->head == NULL) {
            table->head = new_record;
            table->tail = new_record;
        } else {
            new_record->next = table->head;
            table->head->prev = new_record;
            table->head = new_record;
        }

        for (int i = 0; i < table->num_columns; i++) {
            free(values[i]);
        }
        free(values);
    }

    cJSON_Delete(json);
    free(json_data);
    printf("Table '%s' loaded from JSON file '%s'.\n", table->name, filename);
}

// Excel: Save table to an Excel file
void save_to_excel(const char* filename, Table* table) {
    #include "xlsxwriter.h"
    lxw_workbook* workbook = workbook_new(filename);
    lxw_worksheet* worksheet = workbook_add_worksheet(workbook, NULL);

    // Write column headers
    for (int i = 0; i < table->num_columns; i++) {
        worksheet_write_string(worksheet, 0, i, table->columns[i].name, NULL);
    }

    // Write records
    Record* current = table->head;
    int row = 1;
    while (current != NULL) {
        for (int i = 0; i < table->num_columns; i++) {
            if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                worksheet_write_number(worksheet, row, i, *(int*)current->data[i], NULL);
            } else if (strcmp(table->columns[i].type, "STRING") == 0) {
                worksheet_write_string(worksheet, row, i, (char*)current->data[i], NULL);
            }
        }
        row++;
        current = current->next;
    }

    workbook_close(workbook);
    printf("Table '%s' saved to Excel file '%s'.\n", table->name, filename);
}

// Excel: Load table from an Excel file
void load_from_excel(const char* filename, Table* table) {
    #include "xlsxreader.h"
    xlsxioreader xlsxioread = xlsxioread_open(filename);
    if (!xlsxioread) {
        printf("Error: Failed to open Excel file.\n");
        return;
    }

    xlsxioreadersheet sheet = xlsxioread_sheet_open(xlsxioread, NULL, XLSXIOREAD_SKIP_EMPTY_ROWS);
    if (!sheet) {
        printf("Error: Failed to open sheet.\n");
        xlsxioread_close(xlsxioread);
        return;
    }

    char* value;
    int row = 0;
    while (xlsxioread_sheet_next_row(sheet)) {
        char** values = (char**)malloc(table->num_columns * sizeof(char*));  // Dynamically allocate memory
        if (!values) {
            perror("Memory allocation failed");
            xlsxioread_sheet_close(sheet);
            xlsxioread_close(xlsxioread);
            return;
        }

        for (int i = 0; i < table->num_columns; i++) {
            value = xlsxioread_sheet_next_cell(sheet);
            values[i] = value ? _strdup(value) : _strdup("");  // Use _strdup for MSVC compatibility
        }

        if (row > 0) { // Skip header row
            Record* new_record = create_record(table, values);
            if (new_record == NULL) {
                printf("Error: Failed to create record.\n");
                for (int i = 0; i < table->num_columns; i++) {
                    free(values[i]);
                }
                free(values);
                continue;
            }

            if (table->head == NULL) {
                table->head = new_record;
                table->tail = new_record;
            } else {
                new_record->next = table->head;
                table->head->prev = new_record;
                table->head = new_record;
            }
        }

        for (int i = 0; i < table->num_columns; i++) {
            free(values[i]);
        }
        free(values);
        row++;
    }

    xlsxioread_sheet_close(sheet);
    xlsxioread_close(xlsxioread);
    printf("Table '%s' loaded from Excel file '%s'.\n", table->name, filename);
}