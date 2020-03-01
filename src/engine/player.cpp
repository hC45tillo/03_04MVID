#include "engine/player.hpp"
#include <iostream>

Player::~Player() {}

Player::Player(glm::vec3 pos) {
	position = pos;
}

void Player::damage() {
	lifes--;
}

void Player::up(float dt) {
	position.y += dt * speed;

	std::cout << "y: "  << position.y << std::endl;
}
void Player::down(float dt) {
	position.y -= dt * speed;
}
void Player::right(float dt) {
	position.x += dt * speed;
}
void Player::left(float dt) {
	position.x -= dt * speed;
}
void Player::update(float dt)
{
}

