import { useState } from 'react'
import './App.css'

const PLACEHOLDER = `-- Escribe tus sentencias SQL aquí
CREATE DATABASE Universidad;
SET DATABASE Universidad;
CREATE TABLE Estudiante (ID INTEGER, Nombre VARCHAR(30));
INSERT INTO Estudiante VALUES(1, "Juan");
SELECT * FROM Estudiante;`

export default function App() {
  const [query, setQuery]     = useState('')
  const [results, setResults] = useState([])
  const [loading, setLoading] = useState(false)
  const [error, setError]     = useState(null)

  async function runQuery() {
    if (!query.trim()) return
    setLoading(true)
    setError(null)
    setResults([])

    const statements = query
      .split(';')
      .map(s => s.trim())
      .filter(s => s.length > 0)

    const output = []

    for (const stmt of statements) {
      const start = performance.now()
      try {
        const res = await fetch('/api/query', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ query: stmt })
        })
        const data = await res.json()
        const elapsed = (performance.now() - start).toFixed(2)
        output.push({ stmt, data, elapsed, ok: true })
      } catch (e) {
        const elapsed = (performance.now() - start).toFixed(2)
        output.push({ stmt, data: { error: e.message }, elapsed, ok: false })
      }
    }

    setResults(output)
    setLoading(false)
  }

  return (
    <div className="app">
      <header className="header">
        <h1>TinySQLDb</h1>
        <span className="subtitle">Motor de bases de datos relacional</span>
      </header>

      <main className="main">
        <section className="editor-section">
          <label className="section-label">Editor SQL</label>
          <textarea
            className="editor"
            value={query}
            onChange={e => setQuery(e.target.value)}
            placeholder={PLACEHOLDER}
            spellCheck={false}
          />
          <div className="editor-actions">
            <button
              className="btn-run"
              onClick={runQuery}
              disabled={loading || !query.trim()}
            >
              {loading ? 'Ejecutando...' : '▶ Ejecutar'}
            </button>
            <button
              className="btn-clear"
              onClick={() => { setQuery(''); setResults([]); setError(null) }}
            >
              Limpiar
            </button>
          </div>
        </section>

        <section className="results-section">
          <label className="section-label">Resultados</label>
          {results.length === 0 && !loading && (
            <div className="empty">Los resultados aparecerán aquí</div>
          )}
          {results.map((r, i) => (
            <div key={i} className={`result-block ${r.ok ? '' : 'result-error'}`}>
              <div className="result-meta">
                <code className="result-stmt">{r.stmt}</code>
                <span className="result-time">{r.elapsed} ms</span>
              </div>
              {r.ok && r.data.columns && r.data.rows ? (
                <div className="table-wrapper">
                  <table>
                    <thead>
                      <tr>
                        {r.data.columns.map((col, j) => <th key={j}>{col}</th>)}
                      </tr>
                    </thead>
                    <tbody>
                      {r.data.rows.map((row, j) => (
                        <tr key={j}>
                          {row.map((cell, k) => <td key={k}>{cell}</td>)}
                        </tr>
                      ))}
                    </tbody>
                  </table>
                </div>
              ) : (
                <div className="result-message">
                  {r.data.message || r.data.error || 'OK'}
                </div>
              )}
            </div>
          ))}
        </section>
      </main>
    </div>
  )
}