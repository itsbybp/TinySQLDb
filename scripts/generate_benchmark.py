from pathlib import Path

count = 10_000
lines = [
    "CREATE DATABASE Benchmark10000;",
    "SET DATABASE Benchmark10000;",
    "CREATE TABLE Datos (ID INTEGER, Nombre VARCHAR(30), Fecha DATETIME);",
]
lines.extend(
    f'INSERT INTO Datos VALUES ({i}, "Registro{i}", "2026-01-01 00:00:00");'
    for i in range(1, count + 1)
)
lines.extend([
    "SELECT * FROM Datos WHERE ID = 9999;",
    "CREATE INDEX Datos_ID_Index ON Datos(ID) OF TYPE BST;",
    "SELECT * FROM Datos WHERE ID = 9999;",
])
Path(__file__).with_name("bench_10000.sql").write_text("\n".join(lines), encoding="utf-8")
print("Created scripts/bench_10000.sql with 10,000 INSERT statements.")
