#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

class GameObject {
	public:
		virtual ~GameObject() {};
		virtual void update(const float dt) = 0;
	protected:
		GameObject() {};
};

#endif