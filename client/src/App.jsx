import { useState } from 'react';
import './App.css';

const exampleScript = `CREATE DATABASE Universidad;
SET DATABASE Universidad;
CREATE TABLE Estudiante (
  ID INTEGER,
  Nombre VARCHAR(30),
  PrimerApellido VARCHAR(30),
  SegundoApellido VARCHAR(30),
  FechaNacimiento DATETIME
);
INSERT INTO Estudiante VALUES (1, "Isaac", "Ramirez", "Herrera", "2000-01-01 01:02:00");
INSERT INTO Estudiante VALUES (2, "Juan", "Ramirez", "X", "2000-01-01 01:02:00");
CREATE INDEX Estudiante_Id ON Estudiante(ID) OF TYPE BTREE;
SELECT * FROM Estudiante WHERE ID = 2;`;

function ResultTable({ result }) {
  if (!result.rows || result.rows.length === 0) {
    return <p className="empty">No rows returned.</p>;
  }

  const columns = result.columns && result.columns.length > 0
    ? result.columns
    : Object.keys(result.rows[0]);

  return (
    <table>
      <thead>
        <tr>{columns.map((col) => <th key={col}>{col}</th>)}</tr>
      </thead>
      <tbody>
        {result.rows.map((row, i) => (
          <tr key={i}>{columns.map((col) => <td key={col}>{String(row[col] ?? '')}</td>)}</tr>
        ))}
      </tbody>
    </table>
  );
}

export default function App() {
  const [database, setDatabase] = useState('');
  const [script, setScript] = useState(exampleScript);
  const [response, setResponse] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');

  async function executeScript() {
    setLoading(true);
    setError('');
    setResponse(null);

    try {
      const res = await fetch('/query', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
		script: script,
  database: database
	})
      });

      const data = await res.json();
      setResponse(data);
      if (!res.ok && !data.results) {
        throw new Error(data.error || `Server returned HTTP ${res.status}`);
      }
      if (data.database) setDatabase(data.database);
    } catch (err) {
      setError(err.message);
    } finally {
      setLoading(false);
    }
  }

  return (
    <main className="app">
      <section className="header">
        <h1>TinySQLDb</h1>
        <p>Write SQL statements separated by semicolons and execute them through the C++ Web API.</p>
      </section>

      <section className="panel">
        <div className="toolbar">
          <input
            value={database}
            onChange={(e) => setDatabase(e.target.value)}
            placeholder="Current database context"
          />
          <button onClick={executeScript} disabled={loading}>{loading ? 'Running...' : 'Run script'}</button>
        </div>
        <textarea value={script} onChange={(e) => setScript(e.target.value)} />
        {response && <p className={`status ${response.ok ? 'success' : 'error'}`}>{response.ok ? 'Script completed' : 'Script completed with errors'} in {Number(response.elapsedMs || 0).toFixed(3)} ms. Context: {response.database || '(none)'}</p>}
        {error && <p className="error">{error}</p>}
      </section>

      {response?.results?.map((result, index) => (
        <section className="panel result-block" key={index}>
          <div className="result-title">{index + 1}. {result.statement}</div>
          {result.message && <p className="status">{result.message}</p>}
          {result.error && <p className="error"><strong>Error:</strong> {result.error}</p>}
          <p className="timing">Server time: {Number(result.elapsedMs || 0).toFixed(3)} ms</p>
          {!result.error && <ResultTable result={result} />}
        </section>
      ))}
    </main>
  );
}
