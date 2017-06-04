#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <omp.h>

#include <SDL2/SDL.h>

#include <glm/vec2.hpp> //vector 2, used for postion and velocity
#include <glm/vec3.hpp> //vector 3, used for colour

#include <glm/gtx/norm.hpp>

#include <functional>

#include "Particle.h"
#include "HitMarker.h"
#include "HashTable.h"
#include "FPSProfiler.h"

class myScene
{
public:
	myScene(SDL_Renderer* _renderer, float _height, float _width);
	~myScene();

	float GetCurrDT();
	float GetHighestDT();

	void Setdivider(int _div);
	void Restart(int _pNum);

	void CreateRandomParticle();
	void CreateSetParticle(glm::vec2 _pos, glm::vec2 _vel, glm::vec3 _col, float _mass);

	void SetWindowTitle(SDL_Window* _window);

	void ProcessFrame();
	
	void OptimiseFPS();

	void PushParticlesToHashTable();

	void AddParticles(int _pNum);
	void RemoveParticles(int _pNum);
	void AddDividers(int _dNum);
	void RemoveDividers(int _dNum);

	void Update();

	void CheckCollision_BruteForce(); //check collision of a particle with every other particle
	void CheckCollision_HashTable(); //check collision of a particle with every other particle in the cell
	void Collision(myParticle* _p1, myParticle* _p2);
	void EdgeCollision(myParticle* _p);

	void Draw();

	void ToggleHitMarkers() { m_showHitMarkers = !m_showHitMarkers; }
	void ToggleGrid() { m_showGrid = !m_showGrid; }
	
	void ToggleOpenMP() { m_useOpenMP = !m_useOpenMP; }
	void ToggleOptimiseFPS() { m_optimseFPS = !m_optimseFPS; }

	bool GetOptimiseFPS() { return m_optimseFPS; }

	void ToggleThrottleFPS() { m_FPSProfiler->ToggleThrottleFPS(); }

protected:
private:
	SDL_Renderer* m_mainRenderer;

	mySpatialHashingTable* m_hashTable;
	FPSProfiler* m_FPSProfiler;

	std::vector<myParticle*> m_particles;
	std::vector<myHitMarker*> m_hitMarkers;

	//need particles to not leave scene
	float m_height;
	float m_width;

	Uint32 m_lastTimeMS;
	Uint32 m_currentTimeMS;
	float m_deltaTimeS;

	bool m_showHitMarkers;
	bool m_showGrid;

	bool m_ouputNumThreads; //used to make collision check output number of threads used in collision checking
	bool m_useOpenMP;
	bool m_optimseFPS;

	int m_optimisationCounter;

	float m_prevAvgDT;
	int m_frameCounter;
	bool m_increasedBucketNum; //was the last action to increase the number of buckets?
};

#endif // !_SCENE_H_
