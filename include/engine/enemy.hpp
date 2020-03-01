#ifndef __ENEMYH__
#define __ENEMY_H__ 1

#include <cstdint>
#include <engine\model.hpp>
#include <engine\texture.hpp>
#include <engine\GameObject.hpp>

class Enemy {
    public:
		//Model model = Model("../assets/models/tie/TIE_Advanced_X2_MSDOS.obj");
		glm::vec3 position;
		glm::vec3 initialPosition;
		float speed = 0.03f;
		float direction = 1.0f;

		Enemy() = default;
		Enemy(glm::vec3 pos);
		Enemy(glm::vec3 pos, float _speed);
		virtual ~Enemy();
		void update(float dt);
		void damage();
		void falling(float dt);
		void sinFalling(float dt);
		void patrolX(float dt);
};
#endif