#ifndef GLOBALS_H
#define GLOBALS_H

#include "hashmap.h"
#include "table.h"

// Declare global variables
extern HashMap hashmap; // Global hashmap
extern Table* tables[10]; // Global array of tables
extern int num_tables; // Global counter for tables

#endif // GLOBALS_H