#include "engine/lavaBall.hpp"
#include <iostream>

LavaBall::~LavaBall() {}

void LavaBall::update(float dt)
{
	position -= dt * speed;
}

