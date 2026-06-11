const express = require('express');
const cors = require('cors');
const { load, save } = require('./storage');

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
    // Minimal processing: echo and confirm receipt
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
  app.listen(PORT, '0.0.0.0', () => {
    console.log(`Server listening on http://localhost:${PORT}`);
  });
}).catch(err => {
  console.error('Failed to initialize server:', err);
  process.exit(1);
});