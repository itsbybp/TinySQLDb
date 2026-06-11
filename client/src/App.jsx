import { useState } from 'react'
import './App.css'

const PLACEHOLDER = `-- Escribe tus sentencias SQL aquí
CREATE DATABASE Universidad;
SET DATABASE Universidad;
CREATE TABLE Estudiante (ID INTEGER, Nombre VARCHAR(30));
INSERT INTO Estudiante VALUES(1, "Juan");
SELECT * FROM Estudiante;`

export default function App() {
  const [script, setScript] = useState(PLACEHOLDER)
  const [output, setOutput] = useState('')
  const [loading, setLoading] = useState(false)

  async function runScript() {
    setLoading(true)
    setOutput('')
    try {
      const res = await fetch('http://localhost:5000/query', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ sql: script })
      })

      if (!res.ok) {
        const text = await res.text()
        setOutput(`Server error: ${res.status} - ${text}`)
        return
      }

      const data = await res.json()
      setOutput(JSON.stringify(data, null, 2))
    } catch (err) {
      setOutput(`Fetch error: ${err.message}`)
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