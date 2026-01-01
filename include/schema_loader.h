#ifndef SCHEMA_LOADER_H
#define SCHEMA_LOADER_H

#include "table.h"

// Function prototypes for loading table schemas from config files
int load_tables_from_json(const char* filename);
int load_tables_from_yaml(const char* filename);

// Helper function to parse a single table from JSON-like format
Table* parse_table_from_json_string(const char* json_str);

#endif
