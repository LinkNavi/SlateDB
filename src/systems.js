// move system
function movementSystem(entity, { position, velocity }, dt) {
  position.x += velocity.vx * dt;
  position.y += velocity.vy * dt;
}

module.exports = { movementSystem };
