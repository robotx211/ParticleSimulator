#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <glm/vec2.hpp> //vector 2, used for postion and velocity
#include <glm/vec3.hpp> //vector 3, used for colour

#include <stdlib.h>; //for random number gen
#include <time.h>; //for random number gen
#include <iostream>

#pragma pack(1) //reduces size in memory
struct myParticle
{
	myParticle();
	myParticle(int _screenWidth, int _screenHeight);
	myParticle(glm::vec2 _pos, glm::vec2 _vel, glm::vec3 _col, float _mass);
	~myParticle();

	void Update(float _dt);

	void Draw(SDL_Renderer *_renderer);

	//store as floats or ints? (ints will be easier, as particles have no size)
	glm::vec2 m_position;
	glm::vec2 m_velocity;

	//could store as SDL_Color, but glm::vec3 allows portability
	glm::ivec3 m_colour;

	//makes particles be modelled as tiny circles, but drawn as squares
	float m_radius;

	//used for collisions
	float m_mass;
};

#endif // !_PARTICLE_H_
