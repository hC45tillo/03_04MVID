#include "engine/enemy.hpp"
#include <iostream>

Enemy::~Enemy() {}

Enemy::Enemy(glm::vec3 pos) {
	position = pos;
	initialPosition = pos;
	direction = 1.0f;
}
Enemy::Enemy(glm::vec3 pos, float _speed) {
	position = pos;
	initialPosition = pos;
	direction = 1.0f;
	speed = _speed;
}
void Enemy::damage() {
}

void Enemy::update(float dt)
{
}

void Enemy::falling(float dt) {
	position.y -= dt + speed;
}

void Enemy::sinFalling(float dt) {
	if (direction > 0) {
		position.x += 0.5 * sin(glm::radians((360.0f * (dt + speed))));
		if (position.x >= initialPosition.x + 5) {
			direction = -1.0f;
		}
	}
	if (direction < 0) {
		position.x -= 0.5 * sin(glm::radians((360.0f * (dt + speed))));
		if (position.x <= initialPosition.x - 5) {
			direction = 1.0f;
		}
	}
	position.y -= dt + speed;
}

void Enemy::patrolX(float dt) {
	if (direction > 0) {
		position.x += speed + dt;
		if (position.x >= initialPosition.x + 10) {
			direction = -1.0f;
		}
	}
	if (direction < 0) {
		position.x -= speed + dt;
		if (position.x <= initialPosition.x - 10) {
			direction = 1.0f;
		}
	}
}

void Enemy::up(float dt) {
	position.y += dt + speed;
}
