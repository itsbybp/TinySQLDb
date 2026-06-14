import { useState } from 'react'
import './App.css'

const PLACEHOLDER = `-- Escribe tus sentencias SQL aquí
CREATE DATABASE Universidad;
SET DATABASE Universidad;
CREATE TABLE Estudiante (ID INTEGER, Nombre VARCHAR(30));
INSERT INTO Estudiante VALUES(1, "Juan");
SELECT * FROM Estudiante;`

async function sendQuery(sql, database) {
  const resp = await fetch("http://localhost:8080/query", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ sql, context: { database } }),
    mode: "cors"
  });
  if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
  return resp.json();
}

// Ejemplo de función para manejar el clic del botón "Ejecutar"
async function onRunClicked() {
    try {
        // Reemplazar editorText y currentDatabase con los valores actuales de tu estado
        const result = await sendQuery(editorText, currentDatabase);
        setResult(result);    // Actualizar el estado con el resultado para mostrarlo en la tabla
        setError(null);
    } catch (err) {
        setError(err.message || "Network error");
    }
}

export default function App() {
  const [script, setScript] = useState(PLACEHOLDER)
  const [output, setOutput] = useState('')
  const [loading, setLoading] = useState(false)

  async function runScript() {
    setLoading(true)
    setOutput('')
    try {
      const result = await sendQuery(script); // ajusta nombres de estado
      setOutput(JSON.stringify(result, null, 2)); // actualiza el estado para renderizar la tabla
    } catch (err) {
      setOutput(err.message || "Error de red");
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="app">
      <header className="header">
        <h1>TinySQLDb</h1>
        <p className="subtitle">Escribe sentencias SQL y envíalas al servidor</p>
      </header>

      <main className="main">
        <label className="section-label">Editor</label>
        <textarea
          className="editor"
          value={script}
          onChange={e => setScript(e.target.value)}
          rows={12}
        />

        <div style={{ display: 'flex', gap: 12, alignItems: 'center' }}>
          <button onClick={runScript} disabled={loading} className="run-btn">
            {loading ? 'Enviando...' : 'Ejecutar'}
          </button>
        </div>

        <label className="section-label">Salida</label>
        <pre className="output">{output}</pre>
      </main>
    </div>
  )
}