#include "Particle.h"

//default constructor is random position (between (0,0) to (1280, 720) ), random velocity (between (-10,-10) and (10, 10) ) and white for colour
myParticle::myParticle()
{
	//set as 0.5, as to occupy (slightly less than) 1 pixel
	m_radius = ( (float) (rand() % 8) ) + 0.5f;
	//m_radius = 0.5f;

	m_position.x = rand() % 1280;
	m_position.y = rand() % 720;

	m_velocity.x = (rand() % 200) - 100;
	m_velocity.y = (rand() % 200) - 100;



	/*m_colour.r = (rand() % 150) + 100;
	m_colour.g = (rand() % 150) + 100;
	m_colour.b = (rand() % 150) + 100;*/

	m_mass = m_radius; //by default mass = radius, so larger particles are more massive
}

myParticle::myParticle(int _screenWidth, int _screenHeight)
{
	//set as 0.5, as to occupy (slightly less than) 1 pixel
	m_radius = ((float)(rand() % 8)) + 0.5f;
	//m_radius = 0.5f;

	m_position.x = rand() % _screenWidth;
	m_position.y = rand() % _screenHeight;

	m_velocity.x = (rand() % 200) - 100;
	m_velocity.y = (rand() % 200) - 100;

	m_colour.r = (rand() % 150) + 100;
	m_colour.g = (rand() % 150) + 100;
	m_colour.b = (rand() % 150) + 100;

	m_mass = m_radius; //by default mass = radius, so larger particles are more massive
}

myParticle::myParticle(glm::vec2 _pos, glm::vec2 _vel, glm::vec3 _col, float _mass)
{
	m_radius = 0.5f;

	m_position = _pos;

	m_velocity = _vel;

	m_colour = _col;

	m_mass = _mass;
}

myParticle::~myParticle()
{

}

//-----------------------------------------------------------------------------------------------------------------//


void myParticle::Update(float _dt)
{
	//position increases by a percentage of velocity, based on delta time
	m_position += m_velocity * _dt;
}


//-----------------------------------------------------------------------------------------------------------------//


//draws a pixel of set colour to represent the particle
void myParticle::Draw(SDL_Renderer *_renderer)
{
	if (m_radius <= 0.5f)
	{
		//draw as pixel
		SDL_SetRenderDrawColor(_renderer, m_colour.r, m_colour.g, m_colour.b, 255);
		SDL_RenderDrawPoint(_renderer, m_position.x, m_position.y);
	}
	else
	{
		//use SDL2_gfx to draw a circle
		circleRGBA(_renderer, (Sint16)m_position.x, (Sint16)m_position.y, (Sint16)m_radius, (Uint8)m_colour.r, (Uint8)m_colour.g, (Uint8)m_colour.b, (Uint8)255);
	}
	
	
}