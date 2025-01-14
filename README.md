Here’s the updated `README.md` file based on the newest project changes, including the use of `strtok_s` for Windows compatibility, file I/O operations (CSV and binary), and transaction management. The structure and commands have also been updated to reflect the latest features.

---

# ARBAB

## Database Management System (DBMS) Project

This project is a simple Database Management System (DBMS) implemented in C. It supports basic database operations such as creating tables, adding records, deleting records, updating records, and querying data. The system also includes indexing using a Red-Black Tree for efficient data retrieval, file I/O operations (CSV and binary), and transaction management.

---

## Features

- **Table Management**:
  - Create and delete tables.
  - Define columns with specific data types (e.g., `INTEGER`, `STRING`) and constraints (`UNIQUE`, `PRIMARY KEY`, `NOT NULL`).

- **Record Management**:
  - Add, delete, and update records.
  - Validate data (e.g., score ranges for courses).

- **Indexing**:
  - Create an index on the `student-number` column using a Red-Black Tree for faster lookups.

- **Querying**:
  - Select records based on specific criteria.
  - Sort records by `student-number`.

- **File I/O**:
  - Save and load tables to/from CSV files.
  - Save and load tables to/from binary files.

- **Transaction Management**:
  - Begin, commit, and rollback transactions.

- **Help Command**:
  - Provides a list of available commands and usage examples.

---

## Project Structure

The project is organized into the following files and directories:

- **`src/`**: Contains the source code for the project.
  - `command.c`: Handles command parsing and execution.
  - `file_io.c`: Implements file I/O operations (CSV and binary).
  - `hashmap.c`: Implements a hashmap for table storage.
  - `rbtree.c`: Implements a Red-Black Tree for indexing.
  - `record.c`: Manages record creation and deletion.
  - `table.c`: Manages table operations.
  - `utils.c`: Provides utility functions (e.g., sorting, validation).
  - `main.c`: The entry point of the program.

- **`include/`**: Contains header files for the project.
  - `command.h`: Declares command-related functions.
  - `file_io.h`: Declares file I/O-related functions.
  - `hashmap.h`: Declares hashmap-related structures and functions.
  - `rbtree.h`: Declares Red-Black Tree-related structures and functions.
  - `record.h`: Declares record-related structures and functions.
  - `table.h`: Declares table-related structures and functions.
  - `utils.h`: Declares utility functions.

- **`README.md`**: This file, providing an overview of the project.

---

## Getting Started

### Prerequisites

- **C Compiler**: Ensure you have a C compiler installed (e.g., `gcc`, `clang`, or Microsoft Visual Studio).
- **Build System**: CMake (optional, for easier build management).

### Building the Project

#### Using CMake (Recommended)

1. Clone the repository:
   ```bash
   git clone https://github.com/mahahnzavari/ARBAB.git
   cd ARBAB
   ```

2. Create a build directory and compile the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

3. The executable will be generated in the `build` directory.

#### Manual Compilation

1. Navigate to the `src` directory:
   ```bash
   cd src
   ```

2. Compile all source files:
   ```bash
   gcc -o database main.c command.c file_io.c hashmap.c rbtree.c record.c table.c utils.c -I../include
   ```

3. The executable (`database`) will be generated in the `src` directory.

### Running the Project

1. Run the executable:
   ```bash
   ./database
   ```

2. Enter commands in the interactive shell. For example:
   ```
   CREATE TABLE students 3
   ADD students student-number 123 name "John Doe" score 85
   SELECT students score 85 SORTED
   SAVE students data.csv
   EXIT
   ```

3. Use the `HELP` command to see a list of available commands and their usage.

---

## Usage

### Available Commands

1. **Create Table**:
   ```
   CREATE TABLE <table_name> <num_columns>
   ```
   Example:
   ```
   CREATE TABLE students 3
   ```

2. **Delete Table**:
   ```
   DELETE TABLE <table_name>
   ```
   Example:
   ```
   DELETE TABLE students
   ```

3. **Create Index**:
   ```
   CREATE INDEX <table_name>
   ```
   Example:
   ```
   CREATE INDEX students
   ```

4. **Add Record**:
   ```
   ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>
   ```
   Example:
   ```
   ADD students student-number 123 name "John Doe" score 85
   ```

5. **Delete Record**:
   ```
   DELETE <table_name> <column_name> <value>
   ```
   Example:
   ```
   DELETE students student-number 123
   ```

6. **Update Record**:
   ```
   UPDATE <table_name> <column_name> <old_value> <new_value>
   ```
   Example:
   ```
   UPDATE students score 85 90
   ```

7. **Select Records**:
   ```
   SELECT <table_name> <column_name> <value> [SORTED]
   ```
   Example:
   ```
   SELECT students score 85 SORTED
   ```

8. **Select Records with Condition**:
   ```
   SELECT <table_name> WHERE <condition> [SORTED]
   ```
   Example:
   ```
   SELECT students WHERE score > 80 SORTED
   ```

9. **Save Table to CSV**:
   ```
   SAVE <table_name> <filename>
   ```
   Example:
   ```
   SAVE students data.csv
   ```

10. **Load Table from CSV**:
    ```
    LOAD <table_name> <filename>
    ```
    Example:
    ```
    LOAD students data.csv
    ```

11. **Save Table to Binary**:
    ```
    SAVE BINARY <table_name> <filename>
    ```
    Example:
    ```
    SAVE BINARY students data.bin
    ```

12. **Load Table from Binary**:
    ```
    LOAD BINARY <table_name> <filename>
    ```
    Example:
    ```
    LOAD BINARY students data.bin
    ```

13. **Begin Transaction**:
    ```
    BEGIN
    ```

14. **Commit Transaction**:
    ```
    COMMIT
    ```

15. **Rollback Transaction**:
    ```
    ROLLBACK
    ```

16. **Help**:
    ```
    HELP
    ```

17. **Exit**:
    ```
    EXIT
    ```

---

## Example Workflow

1. Create a table:
   ```
   CREATE TABLE students 3
   ```
   Define columns:
   ```
   Enter column 1 name: student-number
   Enter column 1 type (INTEGER or STRING): INTEGER
   Enter column 1 constraints (UNIQUE, PRIMARY KEY, NOT NULL): PRIMARY KEY

   Enter column 2 name: name
   Enter column 2 type (INTEGER or STRING): STRING
   Enter column 2 constraints (UNIQUE, PRIMARY KEY, NOT NULL): NOT NULL

   Enter column 3 name: score
   Enter column 3 type (INTEGER or STRING): INTEGER
   Enter column 3 constraints (UNIQUE, PRIMARY KEY, NOT NULL):
   ```

2. Add a record:
   ```
   ADD students student-number 123 name "John Doe" score 85
   ```

3. Query records:
   ```
   SELECT students score 85 SORTED
   ```

4. Save the table to a CSV file:
   ```
   SAVE students data.csv
   ```

5. Exit the program:
   ```
   EXIT
   ```

---

## Contributing

Contributions are welcome! If you'd like to contribute, please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature or bugfix.
3. Commit your changes.
4. Push your branch to your fork.
5. Submit a pull request.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Acknowledgments

- This project was developed as part of a learning exercise in C programming and data structures.
- Special thanks to the creators of the Red-Black Tree algorithm for providing an efficient indexing mechanism.

---

## Contact

If you have any questions or suggestions, feel free to reach out:

- **Your Name**: [Your Email](mailto:mahanzavari@gmail.com)
- **GitHub**: [Your GitHub Profile](https://github.com/mahanzavari)

---

Thank you for using my naive database! 🚀

