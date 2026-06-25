CREATE DATABASE DemoCompleta;
SET DATABASE DemoCompleta;
CREATE TABLE Estudiante (
  ID INTEGER,
  Nombre VARCHAR(30),
  Promedio DOUBLE,
  FechaNacimiento DATETIME
);
INSERT INTO Estudiante VALUES (1, "Ana", 91.5, "2001-03-14 08:30:00");
INSERT INTO Estudiante VALUES (2, "Bruno", 78.25, "2000-11-05 14:15:00");
INSERT INTO Estudiante VALUES (3, "Carla", 88.75, "2002-07-19 09:45:00");
SELECT * FROM Estudiante;
SELECT ID, Nombre FROM Estudiante WHERE ID = 3;
SELECT * FROM Estudiante WHERE ID > 1;
SELECT * FROM Estudiante WHERE ID < 3;
SELECT * FROM Estudiante WHERE Nombre LIKE *a*;
SELECT * FROM Estudiante WHERE Nombre NOT "Ana";
SELECT * FROM Estudiante ORDER BY Nombre ASC;
SELECT * FROM Estudiante ORDER BY Nombre DESC;
UPDATE Estudiante SET Nombre = "Carlos" WHERE ID = 3;
DELETE FROM Estudiante WHERE ID = 1;
CREATE INDEX Estudiante_ID ON Estudiante(ID) OF TYPE BST;
SELECT * FROM Estudiante WHERE ID = 2;
SELECT * FROM SystemDatabases;
SELECT * FROM SystemTables;
SELECT * FROM SystemColumns;
SELECT * FROM SystemIndexes;
