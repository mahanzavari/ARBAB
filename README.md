# ARBAB Database

A simple database system implemented in C, featuring a hashmap for table storage and a Red-Black Tree for indexing. This project is designed to demonstrate basic database operations such as creating tables, adding records, deleting records, updating records, and querying data.

---

## Features

- **Table Management**:
  - Create and delete tables.
  - Add, update, and delete records.
- **Indexing**:
  - Red-Black Tree indexing on the `student-number` column for fast lookups.
- **Querying**:
  - Select records based on conditions.
  - Sort records by `student-number`.
- **Data Structures**:
  - Hashmap for storing tables.
  - Linked list for storing records.
  - Red-Black Tree for indexing.

---

## Project Structure

```
ARBAB/
├── include/               # Header files
│   ├── commands.h         # Command-related functions
│   ├── table.h            # Table and record structures
│   ├── hashmap.h          # Hashmap implementation
│   ├── rbtree.h           # Red-Black Tree implementation
│   ├── utils.h            # Utility functions
│   ├── globals.h          # Global variables
├── src/                   # Source files
│   ├── ARBAB_Database.c   # Main program
│   ├── commands.c         # Command implementations
│   ├── table.c            # Table and record functions
│   ├── hashmap.c          # Hashmap functions
│   ├── rbtree.c           # Red-Black Tree functions
│   ├── utils.c            # Utility functions
├── bin/                   # Compiled executable
├── .vscode/               # VSCode configuration files
│   ├── tasks.json         # Build task configuration
│   ├── launch.json        # Debug configuration
├── Makefile               # Build automation
├── README.md              # Project documentation
```

---

## Commands

The database supports the following commands:

### Table Management
- **Create Table**:
  ```bash
  CREATE TABLE <table_name>
  ```
  Creates a new table with predefined columns:
  - `student-number` (INTEGER)
  - `general-course-name` (STRING)
  - `general-course-instructor` (STRING)
  - `general-course-score` (INTEGER)
  - `core-course-name` (STRING)
  - `core-course-instructor` (STRING)
  - `core-course-score` (INTEGER)

- **Delete Table**:
  ```bash
  DELETE TABLE <table_name>
  ```
  Deletes an existing table and all its records.

### Record Management
- **Add Record**:
  ```bash
  ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>
  ```
  Adds a new record to the specified table.

- **Delete Record**:
  ```bash
  DELETE <table_name> <column_name> <value>
  ```
  Deletes all records in the specified table where the column matches the value.

- **Update Record**:
  ```bash
  UPDATE <table_name> <column_name> <old_value> <new_value>
  ```
  Updates all records in the specified table where the column matches the old value.

### Querying
- **Select Records**:
  ```bash
  SELECT <table_name> <column_name> <value> [SORTED]
  ```
  Selects all records in the specified table where the column matches the value. If `SORTED` is specified, the results are sorted by `student-number`.

### Indexing
- **Create Index**:
  ```bash
  CREATE INDEX <table_name>
  ```
  Creates a Red-Black Tree index on the `student-number` column for the specified table.

---

## Building the Project

### Prerequisites
- **GCC**: Ensure you have GCC installed on your system.
- **Make**: Optional, but recommended for building the project.

### Using Makefile
1. Open a terminal in the project root directory.
2. Run the following command to build the project:
   ```bash
   make
   ```
   This will compile all source files and create an executable in the `bin/` directory.

3. To clean the build artifacts, run:
   ```bash
   make clean
   ```

### Manual Build
If you prefer to build manually, run:
```bash
gcc -Wall -Wextra -g3 -Iinclude src/ARBAB_Database.c src/commands.c src/table.c src/hashmap.c src/rbtree.c src/utils.c -o bin/ARBAB_Database
```

---

## Running the Project

1. After building the project, navigate to the `bin/` directory:
   ```bash
   cd bin
   ```

2. Run the executable:
   ```bash
   ./ARBAB_Database
   ```

3. Enter commands interactively. For example:
   ```bash
   CREATE TABLE students
   ADD students student-number 123 general-course-name "DSA" general-course-instructor "Dr. ARBAB" general-course-score 85 core-course-name "Physics" core-course-instructor "Dr. Zeinali" core-course-score 90
   SELECT students student-number 123
   ```

4. To exit the program, type:
   ```bash
   exit
   ```

---

## Debugging in VSCode

1. Open the project in VSCode.
2. Set breakpoints in your code.
3. Press `F5` to start debugging.
4. The program will run, and you can inspect variables, step through code, and debug issues.

---

## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, please open an issue or submit a pull request.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Acknowledgments

- Red-Black Tree implementation inspired by [Introduction to Algorithms](https://mitpress.mit.edu/books/introduction-algorithms) by Cormen, Leiserson, Rivest, and Stein.
- Hashmap implementation based on open-source examples.

---

## Author

- **ARBAB**  
  GitHub: [mahanzavari](https://github.com/mahanzavari)  
  Email: mahanzavari@gmail.com

