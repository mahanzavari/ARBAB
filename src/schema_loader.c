#include "../include/schema_loader.h"
#include "../include/table.h"
#include "../include/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern HashMap hashmap;

// Simple JSON parser for table schema
// Expected format:
// {
//   "tables": [
//     {
//       "name": "students",
//       "columns": [
//         {"name": "id", "type": "INTEGER", "constraints": ["PRIMARY_KEY", "NOT_NULL"]},
//         {"name": "name", "type": "STRING", "constraints": ["NOT_NULL"]},
//         {"name": "score", "type": "FLOAT", "constraints": []}
//       ]
//     }
//   ]
// }

// Helper function to skip whitespace
static void skip_whitespace(const char** ptr) {
    while (**ptr && isspace(**ptr)) {
        (*ptr)++;
    }
}

// Helper function to parse a string value (between quotes)
static char* parse_string(const char** ptr) {
    skip_whitespace(ptr);
    if (**ptr != '"') return NULL;
    
    (*ptr)++; // Skip opening quote
    const char* start = *ptr;
    
    while (**ptr && **ptr != '"') {
        (*ptr)++;
    }
    
    int len = *ptr - start;
    char* result = (char*)malloc(len + 1);
    strncpy(result, start, len);
    result[len] = '\0';
    
    if (**ptr == '"') (*ptr)++; // Skip closing quote
    
    return result;
}

// Helper function to find a key in JSON
static const char* find_key(const char* json, const char* key) {
    char search[256];
    snprintf(search, sizeof(search), "\"%s\"", key);
    return strstr(json, search);
}

// Parse constraints from JSON array
static void parse_constraints(const char* json, ColumnDef* col) {
    col->is_unique = 0;
    col->is_primary_key = 0;
    col->is_not_null = 0;
    col->is_indexed = false;
    
    if (strstr(json, "UNIQUE") || strstr(json, "\"unique\"")) {
        col->is_unique = 1;
    }
    if (strstr(json, "PRIMARY_KEY") || strstr(json, "\"primary_key\"")) {
        col->is_primary_key = 1;
    }
    if (strstr(json, "NOT_NULL") || strstr(json, "\"not_null\"")) {
        col->is_not_null = 1;
    }
}

// Load tables from JSON file
int load_tables_from_json(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open JSON file '%s'\n", filename);
        return -1;
    }
    
    // Read entire file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* json = (char*)malloc(file_size + 1);
    if (!json) {
        fclose(file);
        return -1;
    }
    
    fread(json, 1, file_size, file);
    json[file_size] = '\0';
    fclose(file);
    
    // Find "tables" array
    const char* tables_start = find_key(json, "tables");
    if (!tables_start) {
        printf("Error: No 'tables' key found in JSON\n");
        free(json);
        return -1;
    }
    
    // Move to array opening
    tables_start = strchr(tables_start, '[');
    if (!tables_start) {
        printf("Error: Invalid JSON format\n");
        free(json);
        return -1;
    }
    
    const char* ptr = tables_start + 1;
    int table_count = 0;
    
    // Parse each table
    while (*ptr) {
        skip_whitespace(&ptr);
        if (*ptr == ']') break; // End of tables array
        if (*ptr == ',') {
            ptr++;
            continue;
        }
        if (*ptr != '{') {
            ptr++;
            continue;
        }
        
        // Found a table object
        const char* table_start = ptr;
        int brace_count = 0;
        const char* table_end = ptr;
        
        // Find matching closing brace
        do {
            if (*table_end == '{') brace_count++;
            if (*table_end == '}') brace_count--;
            table_end++;
        } while (brace_count > 0 && *table_end);
        
        // Extract table JSON
        int table_len = table_end - table_start;
        char* table_json = (char*)malloc(table_len + 1);
        strncpy(table_json, table_start, table_len);
        table_json[table_len] = '\0';
        
        // Parse table name
        const char* name_key = find_key(table_json, "name");
        if (!name_key) {
            free(table_json);
            ptr = table_end;
            continue;
        }
        
        name_key = strchr(name_key, ':');
        if (!name_key) {
            free(table_json);
            ptr = table_end;
            continue;
        }
        name_key++;
        
        char* table_name = parse_string(&name_key);
        if (!table_name) {
            free(table_json);
            ptr = table_end;
            continue;
        }
        
        // Check if table already exists
        if (find_table(table_name)) {
            printf("Warning: Table '%s' already exists, skipping\n", table_name);
            free(table_name);
            free(table_json);
            ptr = table_end;
            continue;
        }
        
        // Parse columns
        const char* columns_key = find_key(table_json, "columns");
        if (!columns_key) {
            printf("Error: No columns defined for table '%s'\n", table_name);
            free(table_name);
            free(table_json);
            ptr = table_end;
            continue;
        }
        
        columns_key = strchr(columns_key, '[');
        if (!columns_key) {
            printf("Error: Invalid columns format for table '%s'\n", table_name);
            free(table_name);
            free(table_json);
            ptr = table_end;
            continue;
        }
        
        // Count columns
        int num_columns = 0;
        const char* temp = columns_key;
        while (*temp && *temp != ']') {
            if (*temp == '{') num_columns++;
            temp++;
        }
        
        if (num_columns == 0 || num_columns > MAX_COLUMNS) {
            printf("Error: Invalid number of columns for table '%s'\n", table_name);
            free(table_name);
            free(table_json);
            ptr = table_end;
            continue;
        }
        
        // Create table
        Table* new_table = (Table*)malloc(sizeof(Table));
        if (!new_table) {
            free(table_name);
            free(table_json);
            ptr = table_end;
            continue;
        }
        
        strncpy(new_table->name, table_name, MAX_TABLE_NAME_LENGTH - 1);
        new_table->name[MAX_TABLE_NAME_LENGTH - 1] = '\0';
        new_table->num_columns = num_columns;
        new_table->head = NULL;
        new_table->tail = NULL;
        new_table->index_tree = NULL;
        new_table->indexed_column_index = -1;
        
        new_table->columns = (ColumnDef*)malloc(num_columns * sizeof(ColumnDef));
        if (!new_table->columns) {
            free(new_table);
            free(table_name);
            free(table_json);
            ptr = table_end;
            continue;
        }
        
        // Parse each column
        const char* col_ptr = columns_key + 1;
        int col_idx = 0;
        
        while (*col_ptr && *col_ptr != ']' && col_idx < num_columns) {
            skip_whitespace(&col_ptr);
            if (*col_ptr == ',') {
                col_ptr++;
                continue;
            }
            if (*col_ptr != '{') {
                col_ptr++;
                continue;
            }
            
            // Find column object
            const char* col_start = col_ptr;
            int col_braces = 0;
            const char* col_end = col_ptr;
            
            do {
                if (*col_end == '{') col_braces++;
                if (*col_end == '}') col_braces--;
                col_end++;
            } while (col_braces > 0 && *col_end);
            
            int col_len = col_end - col_start;
            char* col_json = (char*)malloc(col_len + 1);
            strncpy(col_json, col_start, col_len);
            col_json[col_len] = '\0';
            
            // Parse column name
            const char* col_name_key = find_key(col_json, "name");
            if (col_name_key) {
                col_name_key = strchr(col_name_key, ':');
                if (col_name_key) {
                    col_name_key++;
                    char* col_name = parse_string(&col_name_key);
                    if (col_name) {
                        strncpy(new_table->columns[col_idx].name, col_name, MAX_COLUMN_NAME_LENGTH - 1);
                        new_table->columns[col_idx].name[MAX_COLUMN_NAME_LENGTH - 1] = '\0';
                        free(col_name);
                    }
                }
            }
            
            // Parse column type
            const char* col_type_key = find_key(col_json, "type");
            if (col_type_key) {
                col_type_key = strchr(col_type_key, ':');
                if (col_type_key) {
                    col_type_key++;
                    char* col_type = parse_string(&col_type_key);
                    if (col_type) {
                        strncpy(new_table->columns[col_idx].type, col_type, 9);
                        new_table->columns[col_idx].type[9] = '\0';
                        free(col_type);
                    }
                }
            }
            
            // Parse constraints
            parse_constraints(col_json, &new_table->columns[col_idx]);
            
            free(col_json);
            col_ptr = col_end;
            col_idx++;
        }
        
        // Add table to hashmap
        hashmap_insert(&hashmap, table_name, new_table);
        printf("Table '%s' created from JSON with %d columns\n", table_name, num_columns);
        table_count++;
        
        free(table_name);
        free(table_json);
        ptr = table_end;
    }
    
    free(json);
    printf("Successfully loaded %d table(s) from JSON\n", table_count);
    return table_count;
}

// Simple YAML parser (basic implementation)
int load_tables_from_yaml(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open YAML file '%s'\n", filename);
        return -1;
    }
    
    char line[1024];
    Table* current_table = NULL;
    int col_idx = 0;
    int table_count = 0;
    char table_name[MAX_TABLE_NAME_LENGTH] = "";
    int parsing_columns = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        // Skip empty lines and comments
        if (line[0] == '\0' || line[0] == '#') continue;
        
        // Check for table name
        if (strstr(line, "- name:") == line + 2 || strstr(line, "- name:") == line + 4) {
            // Save previous table if exists
            if (current_table) {
                hashmap_insert(&hashmap, table_name, current_table);
                printf("Table '%s' created from YAML with %d columns\n", table_name, current_table->num_columns);
                table_count++;
            }
            
            // Parse table name
            char* colon = strchr(line, ':');
            if (colon) {
                sscanf(colon + 1, " %s", table_name);
                
                // Check if table exists
                if (find_table(table_name)) {
                    printf("Warning: Table '%s' already exists, skipping\n", table_name);
                    current_table = NULL;
                    continue;
                }
                
                // Create new table
                current_table = (Table*)malloc(sizeof(Table));
                if (current_table) {
                    strncpy(current_table->name, table_name, MAX_TABLE_NAME_LENGTH - 1);
                    current_table->name[MAX_TABLE_NAME_LENGTH - 1] = '\0';
                    current_table->head = NULL;
                    current_table->tail = NULL;
                    current_table->index_tree = NULL;
                    current_table->indexed_column_index = -1;
                    current_table->num_columns = 0;
                    current_table->columns = NULL;
                    col_idx = 0;
                    parsing_columns = 0;
                }
            }
        }
        // Check for columns section
        else if (strstr(line, "columns:") && current_table) {
            parsing_columns = 1;
        }
        // Parse column definition
        else if (parsing_columns && strstr(line, "- name:") && current_table) {
            if (current_table->columns == NULL) {
                // Allocate space for columns (assume max)
                current_table->columns = (ColumnDef*)malloc(MAX_COLUMNS * sizeof(ColumnDef));
            }
            
            if (col_idx < MAX_COLUMNS) {
                char col_name[MAX_COLUMN_NAME_LENGTH];
                char* colon = strchr(line, ':');
                if (colon) {
                    sscanf(colon + 1, " %s", col_name);
                    strncpy(current_table->columns[col_idx].name, col_name, MAX_COLUMN_NAME_LENGTH - 1);
                    current_table->columns[col_idx].name[MAX_COLUMN_NAME_LENGTH - 1] = '\0';
                    
                    // Initialize defaults
                    strcpy(current_table->columns[col_idx].type, "STRING");
                    current_table->columns[col_idx].is_unique = 0;
                    current_table->columns[col_idx].is_primary_key = 0;
                    current_table->columns[col_idx].is_not_null = 0;
                    current_table->columns[col_idx].is_indexed = false;
                    
                    current_table->num_columns++;
                    col_idx++;
                }
            }
        }
        // Parse column type
        else if (strstr(line, "type:") && current_table && col_idx > 0) {
            char col_type[20];
            char* colon = strchr(line, ':');
            if (colon) {
                sscanf(colon + 1, " %s", col_type);
                strncpy(current_table->columns[col_idx - 1].type, col_type, 9);
                current_table->columns[col_idx - 1].type[9] = '\0';
            }
        }
        // Parse constraints
        else if (strstr(line, "primary_key: true") || strstr(line, "primary_key: 1")) {
            if (current_table && col_idx > 0) {
                current_table->columns[col_idx - 1].is_primary_key = 1;
            }
        }
        else if (strstr(line, "unique: true") || strstr(line, "unique: 1")) {
            if (current_table && col_idx > 0) {
                current_table->columns[col_idx - 1].is_unique = 1;
            }
        }
        else if (strstr(line, "not_null: true") || strstr(line, "not_null: 1")) {
            if (current_table && col_idx > 0) {
                current_table->columns[col_idx - 1].is_not_null = 1;
            }
        }
    }
    
    // Save last table
    if (current_table) {
        hashmap_insert(&hashmap, table_name, current_table);
        printf("Table '%s' created from YAML with %d columns\n", table_name, current_table->num_columns);
        table_count++;
    }
    
    fclose(file);
    printf("Successfully loaded %d table(s) from YAML\n", table_count);
    return table_count;
}
