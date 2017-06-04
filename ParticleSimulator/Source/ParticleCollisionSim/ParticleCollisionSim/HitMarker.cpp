#include "HitMarker.h"

myHitMarker::myHitMarker()
{
	//unused
}

myHitMarker::myHitMarker(glm::vec2 _pos)
{
	m_position = _pos;
	m_currLifeTime = 0.0f;
	m_maxLifeTime = 0.2f;
	m_delete = false;
	m_size = 10.0f;
}

myHitMarker::~myHitMarker()
{

}


//------------------------------------------------------------//

//tick up lifetime, until max life is exceeded
void myHitMarker::Update(float _dt)
{
	m_currLifeTime += _dt;

	if (m_currLifeTime >= m_maxLifeTime)
	{
		m_delete = true;
	}
}


//------------------------------------------------------------//


void myHitMarker::Draw(SDL_Renderer *_renderer)
{
	SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 0);

	SDL_RenderDrawLine(_renderer,
		m_position.x + m_size,
		m_position.y,
		m_position.x - m_size,
		m_position.y);

	SDL_RenderDrawLine(_renderer,
		m_position.x,
		m_position.y - m_size,
		m_position.x,
		m_position.y + m_size);
}
