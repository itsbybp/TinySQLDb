const fs = require('fs').promises;
const path = require('path');

const DB_PATH = path.join(__dirname, '..', 'data', 'db.json');

async function load() {
  try {
    const raw = await fs.readFile(DB_PATH, 'utf8');
    return JSON.parse(raw);
  } catch {
    return { messages: [] };
  }
}

async function save(state) {
  await fs.mkdir(path.dirname(DB_PATH), { recursive: true });
  await fs.writeFile(DB_PATH, JSON.stringify(state, null, 2), 'utf8');
}

module.exports = { load, save, DB_PATH };