#ifndef __LAVABALLH__
#define __LAVABALL_H__ 1

#include <cstdint>
#include <engine\model.hpp>
#include <engine\texture.hpp>
#include <engine\GameObject.hpp>
#include <engine\geometry\sphere.hpp>

class LavaBall {
    public:
		const Sphere sphere = Sphere(1.0f, 25, 25);
		const Texture texture = Texture("../assets/textures/xwing.jpg", Texture::Format::RGB);
		glm::vec3 position;
		float speed = 4.5f;

		LavaBall() = default;
		virtual ~LavaBall();
		void update(float dt);
};
#endif