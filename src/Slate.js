class WebECS {
  constructor({ persistenceAdapter } = {}) {
    this.entities = new Set(); // all entity IDs
    this.components = new Map(); // Map<componentType, Map<entityId, data>>
    this.nextId = 1;
    this.systems = []; // list of registered systems
    this.persistence = persistenceAdapter; // optional persistence layer
  }

  // ===== Entity =====
  createEntity() {
    const id = this.nextId++;
    this.entities.add(id);
    return id;
  }

  destroyEntity(id) {
    this.entities.delete(id);
    for (let comp of this.components.values()) comp.delete(id);
  }

getEntity(id) {
		return this.entites.get(id);
	}
  // ===== Components =====
  addComponent(entityId, type, data) {
    if (!this.components.has(type)) this.components.set(type, new Map());
    this.components.get(type).set(entityId, data);
  }

  removeComponent(entityId, type) {
    this.components.get(type)?.delete(entityId);
  }

  getComponent(entityId, type) {
    return this.components.get(type)?.get(entityId);
  }

  query(...types) {
    const results = [];
    for (let entity of this.entities) {
      if (types.every(t => this.components.get(t)?.has(entity))) {
        const comps = {};
        for (let t of types) comps[t] = this.components.get(t).get(entity);
        results.push({ entity, components: comps });
      }
    }
    return results;
  }

  // ===== Systems =====
  registerSystem(name, { components, run }) {
    this.systems.push({ name, components, run });
  }

  runSystems(dt) {
    for (let system of this.systems) {
      const entities = this.query(...system.components);
      for (let { entity, components } of entities) {
        system.run(entity, components, dt);
      }
    }
  }

  // ===== Persistence (optional) =====
  async save() {
    if (this.persistence?.save) await this.persistence.save(this);
  }

  async load() {
    if (this.persistence?.load) await this.persistence.load(this);
  }
}

// ===== Export =====
module.exports = WebECS;
