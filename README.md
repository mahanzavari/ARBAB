# ARBAB

## Database Management System (DBMS) Project

This project is a simple Database Management System (DBMS) implemented in C. It supports basic database operations such as creating and deleting tables, adding, updating, and deleting records, and querying data. The system also includes features like indexing using Red-Black Trees, transaction management, and file I/O for saving and loading data in CSV and binary formats.

## Features

### Table Management
- **Create Tables**: Define tables with columns, specifying data types (INTEGER, STRING) and constraints (UNIQUE, PRIMARY KEY, NOT NULL).
- **Delete Tables**: Remove tables and all associated records from the database.

### Record Management
- **Add Records**: Insert new records into a table, ensuring constraints like UNIQUE and NOT NULL are respected.
- **Update Records**: Modify existing records based on specified conditions.
- **Delete Records**: Remove records that match a given condition.
- **Query Records**: Select records based on conditions, with optional sorting by a specific column.

### Indexing
- **Red-Black Tree Indexing**: Efficiently index records using Red-Black Trees, particularly for primary key columns (e.g., student-number), enabling fast search operations.

### Transaction Management
- **Begin Transaction**: Start a transaction to ensure atomicity and consistency.
- **Commit Transaction**: Save changes made during the transaction.
- **Rollback Transaction**: Undo changes made during the transaction, restoring the database to its previous state.

### File I/O
- **CSV Support**: Save and load tables in CSV format for easy data exchange with other applications.
- **Binary Support**: Save and load tables in binary format for faster I/O operations and compact storage.

### Utilities
- **Validation**: Ensure data integrity by validating inputs, such as checking if scores are within the valid range (0 to 20).
- **Sorting**: Sort records using merge sort, allowing for efficient retrieval of sorted data.
- **Parsing**: Parse user commands to extract table names, column names, and values for various operations.

## Project Structure

The project is organized into the following key files:

- **`command.c`**: Handles user commands and interactions.
- **`file_io.c`**: Manages saving and loading tables to/from files.
- **`hashmap.c`**: Implements a hashmap for storing and retrieving tables.
- **`main.c`**: The main program loop for processing user input.
- **`rbtree.c`**: Implements Red-Black Trees for indexing.
- **`record.c`**: Manages record creation and deletion.
- **`table.c`**: Implements table-related functions.
- **`utils.c`**: Provides utility functions like sorting and validation.

## Usage

To compile and run the project:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/your-username/dbms-project.git
   cd dbms-project
   ```

2. **Compile the project**:
   ```bash
   gcc -o dbms main.c command.c file_io.c hashmap.c rbtree.c record.c table.c utils.c
   ```

3. **Run the program**:
   ```bash
   ./dbms
   ```

4. **Use the following commands** to interact with the database:

   - **Create a table**:
     ```
     CREATE TABLE <table_name> <num_columns>
     ```

   - **Add a record**:
     ```
     ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>
     ```

   - **Query records**:
     ```
     SELECT <table_name> <column_name> <value> [SORTED]
     ```

   - **Save a table**:
     ```
     SAVE <table_name> <filename>
     ```

   - **Begin a transaction**:
     ```
     BEGIN
     ```

   - **Exit the program**:
     ```
     exit
     ```

## Dependencies

- **C Standard Library**: The project uses standard C libraries such as `stdio.h`, `stdlib.h`, and `string.h`.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.

## Acknowledgments

- This project was developed as part of a learning exercise in C programming and database management.
- Special thanks to the open-source community for providing valuable resources and inspiration.

## Contact

For any questions, suggestions, or feedback, feel free to reach out:

- **Email**: [mahanzavari@gmail.com](mailto:mahanzavari@gmail.com)
- **GitHub Issues**: [Open an Issue](https://github.com/mahanzavari/ARBAB/issues)
- **Pull Requests**: Contributions are welcome! Please submit a pull request with your changes.

---

Feel free to contact us if you have any questions or need further assistance!

