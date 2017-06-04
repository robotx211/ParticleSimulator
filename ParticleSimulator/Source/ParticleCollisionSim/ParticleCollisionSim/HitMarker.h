#ifndef _HIT_MARKER_H_
#define _HIT_MARKER_H_

#include <SDL2/SDL.h>

#include <glm/vec2.hpp> //vector 2, used for postion and velocity
#include <glm/vec3.hpp> //vector 3, used for colour

#pragma pack(1) //reduces size in memory
struct myHitMarker
{
	myHitMarker();
	myHitMarker(glm::vec2 _pos);
	~myHitMarker();

	void Update(float _dt);

	void Draw(SDL_Renderer *_renderer);

	glm::vec2 m_position;

	glm::vec3 m_colour;

	float m_size;

	float m_maxLifeTime;
	float m_currLifeTime;

	bool m_delete;
};

#endif // !_HIT_MARKER_H_
