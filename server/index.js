const express = require('express');
const cors = require('cors');
const { load, save } = require('./storage');
const https = require('https');
const fs = require('fs');

const app = express();
app.use(cors());
app.use(express.json());

let state = { messages: [] };

async function init() {
  state = await load();
}

app.post('/query', async (req, res) => {
  try {
    const payload = req.body ?? {};
    const entry = {
      id: state.messages.length,
      timestamp: new Date().toISOString(),
      payload
    };
    state.messages.push(entry);
    await save(state);
      // Procesamiento mínimo: solo guarda el mensaje y lo devuelve
    res.json({ ok: true, id: entry.id, received: entry.payload });
  } catch (err) {
    console.error('POST /query error:', err);
    res.status(500).json({ ok: false, error: String(err) });
  }
});

app.get('/messages', (req, res) => {
  res.json({ ok: true, messages: state.messages });
});

const PORT = process.env.PORT || 5000;

init().then(() => {
  // Certificados SSL
  const options = {
    cert: fs.readFileSync('server/certs/cert.pem'),
    key: fs.readFileSync('server/certs/key.pem')
  };

  const server = https.createServer(options, app);
  server.listen(PORT, '0.0.0.0', () => {
    console.log(`Servidor escuchando en https://localhost:${PORT}`);
  });
}).catch(err => {
  console.error('No se pudo inicializar el servidor: ', err);
  process.exit(1);
});