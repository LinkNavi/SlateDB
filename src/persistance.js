// minimal JSON file persistence adapter
const fs = require('fs/promises');
class JSONPersistence {
  constructor(filename = 'db.json') { this.filename = filename; }

  async save(ecs) {
    const data = {
      nextId: ecs.nextId,
      entities: Array.from(ecs.entities),
      components: Array.from(ecs.components.entries()).map(([k,v]) => [k, Array.from(v.entries())])
    };
    await fs.writeFile(this.filename, JSON.stringify(data, null, 2));
  }

  async load(ecs) {
    const raw = await fs.readFile(this.filename, 'utf-8');
    const data = JSON.parse(raw);
    ecs.nextId = data.nextId;
    ecs.entities = new Set(data.entities);
    ecs.components = new Map(data.components.map(([k,v]) => [k, new Map(v)]));
  }
}

module.exports = JSONPersistence;
