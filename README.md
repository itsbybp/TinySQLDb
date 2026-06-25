# TinySQLDb

TinySQLDb is a simple relational database management system developed for **CE2103 — Algorithms and Data Structures II**, First Semester 2026, at the Instituto Tecnológico de Costa Rica.

The project implements a web client in React and a database server in C++17. It supports a subset of SQL, binary and encrypted storage, a shared system catalog, sequential searches, BST and B-Tree indexes, and detailed execution results for every SQL statement.

## Team

* Luis Carlos Bustamante
* Mauricio

## General architecture

```text
React Web Client
        |
        | REST / JSON
        v
Web API
        |
        | SQL text
        v
Query Processor
        |
        | Validated operations
        v
Stored Data Manager
        |
        +--> Binary table files
        +--> System Catalog
        +--> BST / B-Tree indexes in memory
```

### Client

The client is implemented with JavaScript, React and Vite.

Its responsibilities are:

* Provide a text editor for SQL scripts.
* Separate statements using semicolons.
* Send the script and current database context to the Web API.
* Display one result block for each executed statement.
* Render SELECT results as dynamic HTML tables.
* Show detailed errors and server execution time.
* Store the active database context after a successful `SET DATABASE`.

### Web API

The Web API is implemented in C++.

Its responsibilities are:

* Expose the endpoint:

```text
POST /query
```

* Receive and produce JSON.
* Extract the SQL script and database context.
* Call the Query Processor.
* Return detailed results for every statement.
* Configure CORS for the React client.

The Web API is the only server layer that directly manipulates JSON.

### Query Processor

The Query Processor:

* Splits scripts into individual SQL statements.
* Identifies the SQL command.
* Parses syntax.
* Validates identifiers, tables, columns and values.
* Coordinates operations with the Stored Data Manager.
* Measures execution time for each statement.
* Preserves the database context between requests.

### Stored Data Manager

The Stored Data Manager:

* Creates databases as folders.
* Creates binary files for tables.
* Reads and writes table rows.
* Manages the shared system catalog.
* Executes INSERT, SELECT, UPDATE and DELETE.
* Creates and rebuilds indexes.
* Uses indexed offsets for direct record access.
* Updates indexes after data modifications.

## Supported SQL

### CREATE DATABASE

```sql
CREATE DATABASE Universidad;
```

Creates a new database folder and registers it in `SystemDatabases`.

### SET DATABASE

```sql
SET DATABASE Universidad;
```

Validates the database and sets the active context in the client.

### CREATE TABLE

```sql
CREATE TABLE Estudiante (
    ID INTEGER,
    Nombre VARCHAR(30),
    Promedio DOUBLE,
    FechaNacimiento DATETIME
);
```

Supported data types:

* `INTEGER`
* `DOUBLE`
* `VARCHAR(n)`
* `DATETIME`

### DROP TABLE

```sql
DROP TABLE Estudiante;
```

The table can only be dropped when it contains no active rows.

### INSERT

```sql
INSERT INTO Estudiante VALUES (
    1,
    "Ana",
    91.5,
    "2001-03-14 08:30:00"
);
```

The server validates:

* Number of values.
* Data types.
* VARCHAR length.
* Valid DATETIME values.
* Duplicate indexed values.

### SELECT

```sql
SELECT * FROM Estudiante;
```

```sql
SELECT ID, Nombre
FROM Estudiante
WHERE ID = 3;
```

Supported WHERE operators:

```text
=
==
>
<
LIKE
NOT
```

Examples:

```sql
SELECT * FROM Estudiante WHERE ID > 2;
SELECT * FROM Estudiante WHERE Nombre LIKE *an*;
SELECT * FROM Estudiante WHERE Nombre NOT "Ana";
```

### ORDER BY

```sql
SELECT *
FROM Estudiante
ORDER BY Promedio ASC;
```

```sql
SELECT *
FROM Estudiante
ORDER BY Nombre DESC;
```

ORDER BY uses a custom Quicksort implementation.

### UPDATE

```sql
UPDATE Estudiante
SET Nombre = "Carlos"
WHERE ID = 3;
```

Without WHERE, all rows are updated:

```sql
UPDATE Estudiante
SET Nombre = "Actualizado";
```

### DELETE

```sql
DELETE FROM Estudiante
WHERE ID = 4;
```

Without WHERE, all rows are deleted:

```sql
DELETE FROM Estudiante;
```

### CREATE INDEX

```sql
CREATE INDEX Estudiante_ID_Index
ON Estudiante(ID)
OF TYPE BST;
```

Supported index types:

```text
BST
BTREE
```

Only one index is allowed per table.

## System Catalog

The System Catalog is shared by all databases and stores metadata in binary files.

It contains:

* `SystemDatabases`
* `SystemTables`
* `SystemColumns`
* `SystemIndexes`

The catalog can be queried using SELECT:

```sql
SELECT * FROM SystemDatabases;
SELECT * FROM SystemTables;
SELECT * FROM SystemColumns;
SELECT * FROM SystemIndexes;
```

## Binary and encrypted storage

Each database is represented as a folder.

Each table stores:

* Schema information.
* Row information.
* Record offsets.

The files are written in binary format.

A simple XOR transformation is applied when reading and writing the stored bytes, providing a basic encryption layer.

## Index implementation

Indexes are stored in memory.

Each node maps:

```text
indexed value -> file offset
```

The offset identifies the location of the complete record in the binary file.

Without an index, the server performs a sequential scan.

With an index, the server:

1. Searches the BST or B-Tree.
2. Obtains the record offset.
3. Opens the binary file.
4. Moves directly to the record position.
5. Reads the requested row.

Indexes are registered in `SystemIndexes`.

When the server restarts, registered indexes are reconstructed in memory from the stored table data.

### Index restrictions

* Only one index is allowed per table.
* The indexed column must not contain repeated values.
* Future duplicate values are rejected.
* INSERT, UPDATE and DELETE operations update the in-memory index.

## Error response format

The API returns a general response with one result for every executed statement.

Example:

```json
{
  "ok": false,
  "database": "Universidad",
  "elapsedMs": 20.4,
  "results": [
    {
      "statement": "SET DATABASE NoExiste",
      "error": "Database does not exist: NoExiste"
    }
  ]
}
```

Successful SELECT example:

```json
{
  "statement": "SELECT * FROM Estudiante WHERE ID = 2",
  "success": true,
  "message": "SELECT used BST index 'Estudiante_ID_Index'",
  "elapsedMs": 47.7,
  "columns": [
    "ID",
    "Nombre"
  ],
  "rows": [
    {
      "ID": "2",
      "Nombre": "Bruno"
    }
  ]
}
```

Examples of detailed errors:

```text
Database already exists: Universidad
Database does not exist: NoExiste
Table does not exist: Personas
Column does not exist: Apellido
Expected INTEGER, received: abc
VARCHAR(25) value is too long
Invalid DATETIME
Duplicate value violates index
Only one index per table is allowed
Cannot drop table because it contains rows
```

## Project structure

```text
TinySQLDb/
├── client/
│   ├── src/
│   │   ├── App.jsx
│   │   ├── App.css
│   │   ├── main.jsx
│   │   └── index.css
│   ├── index.html
│   ├── package.json
│   └── vite.config.js
│
├── server/
│   ├── include/
│   ├── src/
│   ├── web-api/
│   ├── query-processor/
│   ├── stored-data/
│   └── CMakeLists.txt
│
├── scripts/
│   ├── demo_success.sql
│   ├── demo_errors.sql
│   ├── bench_10000.sql
│   ├── generate_benchmark.py
│   └── run_benchmark.py
│
├── .gitignore
├── LICENSE
└── README.md
```

## Requirements

### Server

* C++17
* CMake 3.20 or newer
* GNU g++ or another compatible compiler

### Client

* Node.js
* npm
* React
* Vite

### Benchmark tools

* Python 3

## Compilation and execution

### Linux / WSL

From the project root:

```bash
rm -rf server/build
cmake -S server -B server/build
cmake --build server/build
```

Run the server:

```bash
cd server
./build/tinysqldb_server
```

The server listens at:

```text
http://localhost:8080
```

In another terminal:

```bash
cd client
npm install
npm run dev
```

The client normally runs at:

```text
http://localhost:5173
```

### Windows PowerShell

From the project root:

```powershell
cmake -S server -B server/build
cmake --build server/build --config Release
```

Run the server:

```powershell
cd server
.\build\Release\tinysqldb_server.exe
```

Run the client in another terminal:

```powershell
cd client
npm install
npm run dev
```

## Test scripts

### Successful operations

```text
scripts/demo_success.sql
```

This script demonstrates valid database, table, row, query and index operations.

### Expected errors

```text
scripts/demo_errors.sql
```

This script demonstrates errors such as:

* Existing database.
* Invalid type.
* Invalid date.
* VARCHAR overflow.
* Duplicate indexed value.
* Missing table.
* Missing column.
* Second index on the same table.
* Dropping a non-empty table.

### Index benchmark

Generate the benchmark:

```bash
python3 scripts/generate_benchmark.py
```

This creates:

```text
scripts/bench_10000.sql
```

The generated script:

1. Creates a database.
2. Creates a table.
3. Inserts 10,000 rows.
4. Searches for `ID = 9999` without an index.
5. Creates a BST index.
6. Repeats the same search with the index.

Run the benchmark:

```bash
python3 scripts/run_benchmark.py
```

Example result:

```text
SELECT used sequential scan
Time: 106.878 ms

SELECT used BST index 'Datos_ID_Index'
Time: 47.782 ms
```

In this execution, the indexed query was approximately 2.24 times faster.

Exact times depend on the computer and current system load.

## Verified functionality

The following features were manually tested:

* Web SQL editor.
* Multiple statements separated by semicolons.
* Results displayed as tables.
* Execution time per statement.
* CREATE DATABASE.
* SET DATABASE.
* CREATE TABLE.
* DROP TABLE.
* INSERT.
* SELECT with specific columns.
* WHERE with `=`, `>`, `<`, `LIKE` and `NOT`.
* ORDER BY ASC and DESC.
* UPDATE with and without WHERE.
* DELETE with and without WHERE.
* BST index.
* B-Tree index.
* Duplicate prevention.
* Indexed direct lookup.
* Index reconstruction after server restart.
* System Catalog SELECT queries.
* Detailed error messages.
* Binary encrypted storage.
* Sequential versus indexed benchmark.

## Important notes

Generated files should not be committed:

```text
server/build/
client/node_modules/
client/dist/
client/.vite/
server/data/
scripts/bench_10000_result.json
```

The external header libraries are:

```text
server/include/httplib.h
server/include/nlohmann/json.hpp
```

They are used for HTTP communication and JSON processing and were not implemented as part of TinySQLDb.

## License

This project uses the MIT License.
