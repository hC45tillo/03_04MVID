#ifndef __PLAYERH__
#define __PLAYER_H__ 1

#include <cstdint>
#include <engine\model.hpp>
#include <engine\texture.hpp>
#include <engine\GameObject.hpp>

class Player {
    public:
		Model model = Model("../assets/models/xwing18/source/xwing_tiefighter_sketchfab.obj");
		Texture texture = Texture("../assets/textures/xwing.jpg", Texture::Format::RGB);
		glm::vec3 position;
		uint32_t lifes = 3;
		float speed = 4.5f;

		Player() = default;
		virtual ~Player();
		void update(float dt);
		void damage();
		void up(float dt);
		void down(float dt);
		void left(float dt);
		void right(float dt);
};
#endif