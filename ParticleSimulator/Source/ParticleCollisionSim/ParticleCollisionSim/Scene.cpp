#include "Scene.h"

#include <iostream> //used just for debugging

myScene::myScene(SDL_Renderer* _renderer, float _height, float _width)
{
	m_mainRenderer = _renderer;
	m_lastTimeMS = SDL_GetTicks();

	m_height = _height;
	m_width = _width;

	m_hashTable = new mySpatialHashingTable(m_height, m_width);
	m_FPSProfiler = new FPSProfiler(60, 60.0f, false);
	//m_hashTable->OutputContents();

	m_ouputNumThreads = false;

	m_showHitMarkers = false;
	m_showGrid = true;

	m_useOpenMP = false;

	m_optimseFPS = false;

	m_optimisationCounter = 0;

	m_prevAvgDT = 0.016666f; //average DT for 60fps
	m_frameCounter = 0;
	m_increasedBucketNum = false;
}

myScene::~myScene()
{
	while (m_particles.size() > 0)
	{
		delete m_particles.at(0);
		m_particles.erase(m_particles.begin());
	}

	while (m_hitMarkers.size() > 0)
	{
		delete m_hitMarkers.at(0);
		m_hitMarkers.erase(m_hitMarkers.begin());
	}	

	m_particles.clear();

	m_mainRenderer = nullptr;
	
	delete m_hashTable;
	delete m_FPSProfiler;
}


//-----------------------------------------------------------------------------------------------------------------//


float myScene::GetCurrDT()
{
	return m_deltaTimeS;
}
float myScene::GetHighestDT()
{
	return m_FPSProfiler->GetHighestFrameDT();
}


//-----------------------------------------------------------------------------------------------------------------//


void myScene::Setdivider(int _div)
{
	m_hashTable->InitTableOfIndex(_div);
}

void myScene::Restart(int _pNum)
{
	m_hashTable->ClearBuckets();

	while (m_particles.size() > 0)
	{
		delete m_particles.at(0);
		m_particles.erase(m_particles.begin());
	}

	while (m_hitMarkers.size() > 0)
	{
		delete m_hitMarkers.at(0);
		m_hitMarkers.erase(m_hitMarkers.begin());
	}

	m_particles.clear();

	for (int i = 0; i < _pNum; i++)
	{
		CreateRandomParticle();
	}

	m_FPSProfiler->Reset();

	m_lastTimeMS = SDL_GetTicks();
}


//-----------------------------------------------------------------------------------------------------------------//


void myScene::CreateRandomParticle()
{
	m_particles.push_back(new myParticle(m_width, m_height));
}

void myScene::CreateSetParticle(glm::vec2 _pos, glm::vec2 _vel, glm::vec3 _col, float _mass)
{
	m_particles.push_back(new myParticle(_pos, _vel, _col, _mass));
}


//-----------------------------------------------------------------------------------------------------------------//


void myScene::SetWindowTitle(SDL_Window* _window)
{
	std::stringstream titleStream;

	titleStream << "Parallel Processing = " << m_useOpenMP << ", Show Grid = " << m_showGrid << ", Show Collisions = " << m_showHitMarkers << ", Optimise FPS = " << m_optimseFPS << ", Throttle FPS = " << m_FPSProfiler->GetThrottleFPSState();
	titleStream << "  |  " << m_particles.size() << " particles, " << m_hashTable->GetDivider() * m_hashTable->GetDivider() << " buckets";
	titleStream << "  |  " << " FPS = " << m_FPSProfiler->GetFPS();

	std::string title(titleStream.str());

	SDL_SetWindowTitle(_window, title.c_str());

}


//-----------------------------------------------------------------------------------------------------------------//


void myScene::ProcessFrame()
{
	//calculate delta time
	m_currentTimeMS = SDL_GetTicks();
	m_deltaTimeS = (float)((m_currentTimeMS - m_lastTimeMS) / 1000.0f);
	m_lastTimeMS = m_currentTimeMS;

	//CheckCollision_BruteForce();

	Update();
	CheckCollision_HashTable();
	Draw();

	m_FPSProfiler->AddDT(m_deltaTimeS);
	m_FPSProfiler->RunFPSCalc();

	//optimse FPS, only once a steady FPS has been found
	if (m_optimseFPS == true && m_FPSProfiler->IsDTHistoryFull())
	{
		if (m_FPSProfiler->GetFPS() < m_FPSProfiler->GetDesiredFPS())
		{
			OptimiseFPS();
		}
		else
		{
			m_optimisationCounter = 0;
		}
	}
	if (m_optimisationCounter > 30 && m_optimseFPS == true)
	{
		std::cout << "Tried to optimise " << m_optimisationCounter << " times and failed to get 60FPS";
		std::cout << "\nSwitching to manual divider control!";

		m_optimisationCounter = 0;

		m_optimseFPS = false;
	}
}


//-----------------------------------------------------------------------------------------------------------------//


void myScene::OptimiseFPS()
{
	//wait x frames for avg to update

	if (m_frameCounter < 20)
	{
		m_frameCounter++;
		return;
	}
	else
	{
		m_frameCounter = 0;

		if (m_FPSProfiler->GetAvgFrameDT() > m_prevAvgDT) //if avg DT is worse than last average taken
		{
			if (m_increasedBucketNum == false) //last was to decrease bucket num
			{
				//therefore increase it
				AddDividers(1);
				m_optimisationCounter++;
				m_increasedBucketNum = true;
			}
			else if (m_increasedBucketNum == true) //last was to increase bucket num
			{
				//therefore decrease it
				RemoveDividers(1);
				m_optimisationCounter++;
				m_increasedBucketNum = false;
			}
		}
		else if (m_FPSProfiler->GetAvgFrameDT() <= m_prevAvgDT) //if avg DT is better than last average taken (but still before desired)
		{
			if (m_increasedBucketNum == false) //last was to decrease bucket num
			{
				//therefore decrease it
				RemoveDividers(1);
				m_optimisationCounter++;
			}
			else if (m_increasedBucketNum == true) //last was to increase bucket num
			{
				//therefore increase it
				AddDividers(1);
				m_optimisationCounter++;
			}
		}
		m_prevAvgDT = m_FPSProfiler->GetAvgFrameDT();
	}
}


//-----------------------------------------------------------------------------------------------------------------//


void myScene::PushParticlesToHashTable()
{
	for (int i = 0; i < m_particles.size(); i++)
	{
		m_hashTable->AddParticle(m_particles.at(i));
	}
}


//-----------------------------------------------------------------------------------------------------------------//


void myScene::AddParticles(int _pNum)
{
	for (int i = 0; i < _pNum; i++)
	{
		CreateRandomParticle();
	}
}

void myScene::RemoveParticles(int _pNum)
{
	//deletes the first x particles
	for (int i = 0; i < _pNum; i++)
	{
		if (m_particles.size() > 0)
		{
			delete m_particles.at(0);
			m_particles.erase(m_particles.begin());
		}
	}
}

void myScene::AddDividers(int _dNum)
{
	Setdivider(m_hashTable->GetDivider() + _dNum);
}

void myScene::RemoveDividers(int _dNum)
{
	if (m_hashTable->GetDivider() - 1 >= _dNum)
	{
		Setdivider(m_hashTable->GetDivider() - _dNum);
	}
	else
	{
		Setdivider(1);
	}
}


//-----------------------------------------------------------------------------------------------------------------//


void myScene::Update()
{
	for each (myParticle* p in m_particles)
	{
		p->Update(m_deltaTimeS);
	}

	for (int i = 0; i < m_hitMarkers.size(); i++)
	{
		m_hitMarkers.at(i)->Update(m_deltaTimeS);

		if (m_hitMarkers.at(i)->m_delete == true)
		{
			delete m_hitMarkers.at(i);
			m_hitMarkers.erase(m_hitMarkers.begin() + i);
			i--;
		}
	}

}


//-----------------------------------------------------------------------------------------------------------------//


//very inefficient, just used to test collisions
void myScene::CheckCollision_BruteForce()
{
	glm::vec2 tempPos;
	float tempDistance;
	if (m_particles.size() > 0)
	{
		for (int i = 0; i < m_particles.size() - 1; i++)
		{
			EdgeCollision(m_particles.at(i));

			tempPos = m_particles.at(i)->m_position;
			for (int j = i + 1; j < m_particles.size(); j++)
			{
				tempDistance = glm::distance(tempPos, m_particles.at(j)->m_position);

				if (tempDistance < m_particles.at(i)->m_radius + m_particles.at(j)->m_radius)
				{
					Collision(m_particles.at(i), m_particles.at(j));
				}
			}
		}
	}
}

void myScene::CheckCollision_HashTable()
{
	m_hashTable->ClearBuckets();
	PushParticlesToHashTable();
	//m_hashTable->OutputContents();

	//for each cell, check collisions on the contents

	//set omp lock so collisions do not write to velocities at same time
	omp_lock_t writeLock;

	omp_init_lock(&writeLock);

	//set num of thread to -1. so it's only written to once
	int numOfThreads = -1;

	if (m_useOpenMP == true)
	{
		//set to use omp to parrelise for loops
		#pragma omp parallel
		{
			//workaround for looping through x and y: http://stackoverflow.com/questions/10540760/openmp-nested-parallel-for-loops-vs-inner-parallel-for

			int maxNum = m_hashTable->GetDivider();

			#pragma omp for
			for (int xy = 0; xy < maxNum*maxNum; ++xy)
			{
				int x = xy / maxNum;
				int y = xy % maxNum;
				//parallelize this code here

				glm::vec2 tempPos;
				float tempDistance;
				std::vector<myParticle*> cellContents;

				if (numOfThreads == -1)
				{
					omp_set_lock(&writeLock);
					numOfThreads = omp_get_num_threads();
					omp_unset_lock(&writeLock);
				}

				cellContents = m_hashTable->GetBucketContents(glm::ivec2(x, y));

				if (cellContents.size() > 0)
				{
					for (int i = 0; i < cellContents.size(); i++)
					{
						EdgeCollision(cellContents.at(i));

						tempPos = cellContents.at(i)->m_position;
						for (int j = i + 1; j < cellContents.size(); j++)
						{
							tempDistance = glm::distance(tempPos, cellContents.at(j)->m_position);

							if (tempDistance < cellContents.at(i)->m_radius + cellContents.at(j)->m_radius)
							{
								Collision(cellContents.at(i), cellContents.at(j));
							}
						}
					}
				}
			}
		}
	}
	else if (m_useOpenMP == false)
	{
		int maxNum = m_hashTable->GetDivider();

		for (int xy = 0; xy < maxNum*maxNum; ++xy)
		{
			int x = xy / maxNum;
			int y = xy % maxNum;

			glm::vec2 tempPos;
			glm::vec2 vectorDiff;
			float tempDistanceSqu;
			std::vector<myParticle*> cellContents;

			if (numOfThreads == -1)
			{
				omp_set_lock(&writeLock);
				numOfThreads = omp_get_num_threads();
				omp_unset_lock(&writeLock);
			}

			cellContents = m_hashTable->GetBucketContents(glm::ivec2(x, y));

			if (cellContents.size() > 0)
			{
				myParticle* iPar;
				myParticle* jPar;
				for (int i = 0; i < cellContents.size(); i++)
				{
					EdgeCollision(cellContents.at(i));

					iPar = cellContents.at(i);
					for (int j = i + 1; j < cellContents.size(); j++)
					{
						
						jPar = cellContents.at(j);

						//code for collisions got from this address:
						//https://gamedevelopment.tutsplus.com/tutorials/when-worlds-collide-simulating-circle-circle-collisions--gamedev-769
						if (iPar->m_position.x + iPar->m_radius + jPar->m_radius > jPar->m_position.x
							&& iPar->m_position.x < jPar->m_position.x + iPar->m_radius + jPar->m_radius
							&&iPar->m_position.y + iPar->m_radius + jPar->m_radius > jPar->m_position.y
							&& iPar->m_position.y < jPar->m_position.y + iPar->m_radius + jPar->m_radius)
						{
							//close enough to check for collision
							Collision(cellContents.at(i), cellContents.at(j));
						}
					}
				}

				iPar = nullptr;
				jPar = nullptr;
			}
		}
	}

	//process -1 key cell
	std::vector<myParticle*> cellContents;
	cellContents = m_hashTable->GetBucketContents(-1);

	if (cellContents.size() > 0)
	{
		for (int i = 0; i < cellContents.size(); i++)
		{
			EdgeCollision(cellContents.at(i));
		}
	}

	//if required, output number of threads
	if (m_ouputNumThreads == true)
	{
		std::cout << "NUMBER OF THREADS = " << numOfThreads << "\n";
		m_ouputNumThreads = false;
	}
}

void myScene::Collision(myParticle* _p1, myParticle* _p2)
{

	//code for collisions got from this address:
	//https://gamedevelopment.tutsplus.com/tutorials/when-worlds-collide-simulating-circle-circle-collisions--gamedev-769


	//don't run collisions at the same time with the same particles
	#pragma omp critical
	{
		//use distance squared rather than distance, square roots are expensive
		float disSquared = ((_p1->m_position.x - _p2->m_position.x) * (_p1->m_position.x - _p2->m_position.x)) + ((_p1->m_position.y - _p2->m_position.y) * (_p1->m_position.y - _p2->m_position.y));

		if (disSquared < (_p1->m_radius + _p2->m_radius) * (_p1->m_radius + _p2->m_radius))
		{
			//balls have collided

			glm::vec2 collPoint;
			collPoint.x =
				((_p1->m_position.x * _p2->m_radius) + (_p2->m_position.x * _p1->m_radius))
				/ (_p1->m_radius + _p2->m_radius);

			collPoint.y =
				((_p1->m_position.y * _p2->m_radius) + (_p2->m_position.y * _p1->m_radius))
				/ (_p1->m_radius + _p2->m_radius);

			if (m_showHitMarkers == true)
			{
				m_hitMarkers.push_back(new myHitMarker(collPoint));
			}

			glm::vec2 newP1Vel, newP2Vel;
			newP1Vel.x = (_p1->m_velocity.x * (_p1->m_mass - _p2->m_mass) + (2 * _p2->m_mass * _p2->m_velocity.x)) / (_p1->m_mass + _p2->m_mass);
			newP1Vel.y = (_p1->m_velocity.y * (_p1->m_mass - _p2->m_mass) + (2 * _p2->m_mass * _p2->m_velocity.y)) / (_p1->m_mass + _p2->m_mass);
			newP2Vel.x = (_p2->m_velocity.x * (_p2->m_mass - _p1->m_mass) + (2 * _p1->m_mass * _p1->m_velocity.x)) / (_p1->m_mass + _p2->m_mass);
			newP2Vel.y = (_p2->m_velocity.y * (_p2->m_mass - _p1->m_mass) + (2 * _p1->m_mass * _p1->m_velocity.y)) / (_p1->m_mass + _p2->m_mass);

			_p1->m_velocity = newP1Vel;
			_p2->m_velocity = newP2Vel;

			_p1->m_position += _p1->m_velocity * m_deltaTimeS;
			_p2->m_position += _p2->m_velocity * m_deltaTimeS;
		}
	}
}

void myScene::EdgeCollision(myParticle* _p)
{
	glm::vec2 tempPos = _p->m_position;
	glm::vec2 tempVel = _p->m_velocity;
	float tempRad = _p->m_radius;

	if (tempPos.x - tempRad <= 0 && tempVel.x < 0)
	{
		_p->m_velocity.x = tempVel.x * -1;
	}
	else if (tempPos.x + tempRad >= m_width && tempVel.x > 0)
	{
		_p->m_velocity.x = tempVel.x * -1;
	}

	if (tempPos.y - tempRad <= 0 && tempVel.y < 0)
	{
		_p->m_velocity.y = tempVel.y * -1;
	}
	else if (tempPos.y + tempRad >= m_height && tempVel.y > 0)
	{
		_p->m_velocity.y = tempVel.y * -1;
	}
}


//-----------------------------------------------------------------------------------------------------------------//


void myScene::Draw()
{
	SDL_SetRenderDrawColor(m_mainRenderer, 0, 0, 0, 0);
	SDL_RenderClear(m_mainRenderer);

	for each (myParticle* p in m_particles)
	{
		p->Draw(m_mainRenderer);
	}

	if (m_showHitMarkers == true)
	{
		for each (myHitMarker* h in m_hitMarkers)
		{
			h->Draw(m_mainRenderer);
		}
	}

	if (m_showGrid == true)
	{
		m_hashTable->DrawBuckets(m_mainRenderer);
	}
	//m_hashTable->OutputContents();

	SDL_RenderPresent(m_mainRenderer);
}