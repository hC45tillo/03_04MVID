#ifndef __QUAD_H__
#define __QUAD_H__ 1

#include "geometry.hpp"
#include "./engine/gameObject.hpp"
class Quad final: public Geometry {
    public:
        Quad() = delete;
        explicit Quad(float size);
		void update(const float dt);

    private:
        float _size;
};
#endif