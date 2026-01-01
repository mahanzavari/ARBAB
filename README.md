# ARBAB Database Management System

A lightweight in-memory database implemented in C with support for multiple data types, B+ Tree indexing, and JOIN operations.

## Features

- **Multiple Data Types**: INTEGER, FLOAT, BOOLEAN, DATE, STRING
- **B+ Tree Indexing**: Optimized for disk I/O with efficient range queries
- **JOIN Operations**: Combine data from multiple tables
- **Schema Loading**: Create tables from JSON or YAML configuration files
- **Transactions**: BEGIN, COMMIT, ROLLBACK support
- **File I/O**: Save/load data in CSV or binary format

## Quick Start

### Build
```bash
mkdir build && cd build
cmake .. && make
./database
```

Or use the Makefile:
```bash
make
./database
```

### Basic Commands

Create a table:
```
CREATE TABLE students 3
```

Add data:
```
ADD students id 1 name "John" gpa 3.85
```

Query data:
```
SELECT students id 1
```

Join tables:
```
JOIN students grades ON students.id = grades.student_id
```

Load schema from file:
```
LOAD SCHEMA example_schema.json
```

## Schema File Example

**JSON** (`example_schema.json`):
```json
{
  "tables": [
    {
      "name": "students",
      "columns": [
        {"name": "id", "type": "INTEGER", "constraints": ["PRIMARY_KEY"]},
        {"name": "name", "type": "STRING", "constraints": ["NOT_NULL"]},
        {"name": "gpa", "type": "FLOAT", "constraints": []}
      ]
    }
  ]
}
```

**YAML** (`example_schema.yaml`):
```yaml
tables:
  - name: students
    columns:
      - name: id
        type: INTEGER
        primary_key: true
      - name: name
        type: STRING
        not_null: true
```

## Command Reference

| Command | Example |
|---------|---------|
| CREATE TABLE | `CREATE TABLE users 3` |
| DELETE TABLE | `DELETE TABLE users` |
| CREATE INDEX | `CREATE INDEX users` |
| ADD | `ADD users id 1 name "Alice"` |
| UPDATE | `UPDATE users name "Alice" "Bob"` |
| DELETE | `DELETE users id 1` |
| SELECT | `SELECT users id 1 SORTED` |
| JOIN | `JOIN t1 t2 ON t1.id = t2.id` |
| LOAD SCHEMA | `LOAD SCHEMA config.json` |
| BEGIN/COMMIT | `BEGIN` → `COMMIT` |
| SAVE/LOAD | `SAVE users data.csv CSV` |
| HELP | `HELP` |
| EXIT | `EXIT` |

## Data Types

- **INTEGER**: Whole numbers
- **FLOAT**: Decimal numbers (3.14, 2.5)
- **BOOLEAN**: `true`, `false`, `1`, or `0`
- **DATE**: Format `YYYY-MM-DD` (e.g., 2024-01-15)
- **STRING**: Text up to 32 characters

## Constraints

- **PRIMARY_KEY**: Unique identifier
- **UNIQUE**: No duplicates
- **NOT_NULL**: Value required

## Project Structure

```
ARBAB/
├── include/          # Header files
├── src/              # Source files
├── example_schema.*  # Example schemas
├── CMakeLists.txt    # CMake build
└── Makefile          # Make build
```

## License

MIT License - see [LICENSE](LICENSE) file

## Author

Mahan Zavari (mahanzavari@gmail.com)

---

Type `HELP` in the database for full command documentation.
