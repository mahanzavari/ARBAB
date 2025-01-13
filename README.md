# ARBAB

## Database Management System (DBMS) Project

This project is a simple Database Management System (DBMS) implemented in C. It supports basic database operations such as creating tables, adding records, deleting records, updating records, and querying data. The system also includes indexing using a Red-Black Tree for efficient data retrieval.

---

## Features

- **Table Management**:
  - Create and delete tables.
  - Define columns with specific data types (e.g., `INTEGER`, `STRING`).
  
- **Record Management**:
  - Add, delete, and update records.
  - Validate data (e.g., score ranges for courses).

- **Indexing**:
  - Create an index on the `student-number` column using a Red-Black Tree for faster lookups.

- **Querying**:
  - Select records based on specific criteria.
  - Sort records by `student-number`.

- **Help Command**:
  - Provides a list of available commands and usage examples.

---

## Project Structure

The project is organized into the following files and directories:

- **`src/`**: Contains the source code for the project.
  - `command.c`: Handles command parsing and execution.
  - `hashmap.c`: Implements a hashmap for table storage.
  - `rbtree.c`: Implements a Red-Black Tree for indexing.
  - `record.c`: Manages record creation and deletion.
  - `table.c`: Manages table operations.
  - `utils.c`: Provides utility functions (e.g., sorting, validation).
  - `main.c`: The entry point of the program.

- **`include/`**: Contains header files for the project.
  - `command.h`: Declares command-related functions.
  - `hashmap.h`: Declares hashmap-related structures and functions.
  - `rbtree.h`: Declares Red-Black Tree-related structures and functions.
  - `record.h`: Declares record-related structures and functions.
  - `table.h`: Declares table-related structures and functions.
  - `utils.h`: Declares utility functions.

- **`README.md`**: This file, providing an overview of the project.

---

## Getting Started

### Prerequisites

- **C Compiler**: Ensure you have a C compiler installed (e.g., `gcc`).
- **Make**: Ensure you have `make` installed for building the project.

### Building the Project

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/your-repo-name.git
   cd your-repo-name
   ```

2. Compile the project:
   ```bash
   make
   ```

   This will generate an executable named `database`.

### Running the Project

1. Run the executable:
   ```bash
   ./database
   ```

2. The program will prompt you to enter commands. Type `HELP` to see a list of available commands and their usage.

---

## Usage

### Available Commands

1. **Create Table**:
   ```bash
   CREATE TABLE <table_name>
   ```
   Example:
   ```bash
   CREATE TABLE students
   ```

2. **Delete Table**:
   ```bash
   DELETE TABLE <table_name>
   ```
   Example:
   ```bash
   DELETE TABLE students
   ```

3. **Create Index**:
   ```bash
   CREATE INDEX <table_name>
   ```
   Example:
   ```bash
   CREATE INDEX students
   ```

4. **Add Record**:
   ```bash
   ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>
   ```
   Example:
   ```bash
   ADD students student-number 12 general-course-name "DSA" general-course-instructor "Dr.ARBAB" general-course-score 85 core-course-name "Physics" core-course-instructor "Dr.zeinali" core-course-score 90
   ```

5. **Delete Record**:
   ```bash
   DELETE <table_name> <column_name> <value>
   ```
   Example:
   ```bash
   DELETE students student-number 12
   ```

6. **Update Record**:
   ```bash
   UPDATE <table_name> <column_name> <old_value> <new_value>
   ```
   Example:
   ```bash
   UPDATE students core-course-score 90 95
   ```

7. **Select Records**:
   ```bash
   SELECT <table_name> <column_name> <value> [SORTED]
   ```
   Example:
   ```bash
   SELECT students core-course-score 90 SORTED
   ```

8. **Help**:
   ```bash
   HELP
   ```

9. **Exit**:
   ```bash
   exit
   ```

---

## Example Workflow

1. Create a table:
   ```bash
   CREATE TABLE students
   ```

2. Add a record:
   ```bash
   ADD students student-number 12 general-course-name "DSA" general-course-instructor "Dr.ARBAB" general-course-score 85 core-course-name "Physics" core-course-instructor "Dr.zeinali" core-course-score 90
   ```

3. Query records:
   ```bash
   SELECT students core-course-score 90 SORTED
   ```

4. Update a record:
   ```bash
   UPDATE students core-course-score 90 95
   ```

5. Delete a record:
   ```bash
   DELETE students student-number 12
   ```

6. Exit the program:
   ```bash
   exit
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

