#include "engine/lavaBall.hpp"
#include <iostream>

LavaBall::~LavaBall() {}

LavaBall::LavaBall(glm::vec3 pos) {
	position = pos;
}

LavaBall::LavaBall(glm::vec3 pos, Geometry geo) {
	geometry = geo;
	position = pos;
}

void LavaBall::update(float dt)
{
	position -= dt * speed;
}

