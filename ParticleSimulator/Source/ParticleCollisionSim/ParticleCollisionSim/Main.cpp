/*
	TODO
Setup Hash Table
Create FPS class to change collision detection intensity and create
*/

#include <SDL2/SDL.h>

#include <stdlib.h>; //for random number gen
#include <time.h>; //for random number gen

#include "Scene.h"

#undef main;

#define FPS_DELAY 1000/60

int main()
{
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;

	SDL_Init(SDL_INIT_VIDEO);

	int windowWidth = 1600;
	int windowHeight = 900;

	window = SDL_CreateWindow("ParticleCollisionSim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN); //create window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	SDL_Event ev;

	srand(time(NULL));

	//create new scene
	myScene* currentScene = new myScene(renderer, windowHeight, windowWidth);

	bool runTests = false;

	if (runTests == false)
	{
		//update window title
		Uint32 lastTitleUpdateMS = SDL_GetTicks();
		Uint32 currentTitleUpdateTimeMS = 0;
		float titleUpdateDT = 0.0f;
		float titleUpdateTimeCounter = 0.0f;
		float titleUpdateDelay = 0.5f;

		bool breakout = false;

		currentScene->Setdivider(2);
		currentScene->Restart(2000);
		while (breakout == false)
		{
			currentScene->ProcessFrame();

			currentTitleUpdateTimeMS = SDL_GetTicks();
			titleUpdateDT = (float)((currentTitleUpdateTimeMS - lastTitleUpdateMS) / 1000.0f);
			titleUpdateTimeCounter += titleUpdateDT;
			if (titleUpdateTimeCounter >= titleUpdateDelay)
			{
				currentScene->SetWindowTitle(window);
				titleUpdateTimeCounter = 0.0f;
			}
			lastTitleUpdateMS = currentTitleUpdateTimeMS;

			while (SDL_PollEvent(&ev) != 0)
			{
				if (ev.type == SDL_QUIT)
				{
					breakout = true;
				}
				if (ev.type == SDL_KEYDOWN)
				{
					switch (ev.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						{
							breakout = true;
							break;
						}

						case SDLK_LEFT:
						{
							currentScene->RemoveParticles(500);
							currentScene->SetWindowTitle(window);
							break;
						}
						case SDLK_RIGHT:
						{
							currentScene->AddParticles(500);
							currentScene->SetWindowTitle(window);
							break;
						}

						//if not set to automatically optimise the FPS
						
						case SDLK_DOWN:
						{
							if (currentScene->GetOptimiseFPS() == false)
							{
								currentScene->RemoveDividers(1);
								currentScene->SetWindowTitle(window);
							}
							break;
						}
						case SDLK_UP:
						{
							if (currentScene->GetOptimiseFPS() == false)
							{
								currentScene->AddDividers(1);
								currentScene->SetWindowTitle(window);
							}
							break;
						}

						case SDLK_g:
						{
							currentScene->ToggleGrid();
							break;
						}
						case SDLK_h:
						{
							currentScene->ToggleHitMarkers();
							currentScene->SetWindowTitle(window);
							break;
						}

						case SDLK_p:
						{
							currentScene->ToggleOpenMP();
							currentScene->SetWindowTitle(window);
							break;
						}

						case SDLK_o:
						{
							currentScene->ToggleOptimiseFPS();
							currentScene->SetWindowTitle(window);
							break;
						}

						case SDLK_t:
						{
							currentScene->ToggleThrottleFPS();
							currentScene->SetWindowTitle(window);
							break;
						}
					}
				}
			}

			
		}
	}
	//run tests for individual numbers of particles and cells
	else if (runTests == true)
	{

		currentScene->ToggleOptimiseFPS();

		int particleStartingNum = 2000;
		int particleFinishingNum = 10000;
		int particleIncrementer = 1000;

		int dividerStartingNum = 1;
		int dividerFinishingNum = 4;
		int dividerIncrementer = 1;

		int pRuns = 0;
		int tempPNum = particleFinishingNum;
	
		while (tempPNum >= particleStartingNum)
		{
			tempPNum -= particleIncrementer;
			pRuns++;
		}

		int dRuns = 0;
		int tempDNum = dividerFinishingNum;

		while (tempDNum >= dividerStartingNum)
		{
			tempDNum -= dividerIncrementer;
			dRuns++;
		}

		int numOfRuns = pRuns * dRuns;

		std::cout << "Number of runs: " << numOfRuns << "\n";

		Uint32 lastRunTimeMS = 0;
		Uint32 currentRunTimeMS = 0;
		float runDeltaTime = 0.0f;
		float totalRunTime = 0.0f;
		float runTimeLimit = 5.0f; //in seconds

		std::cout << "Estimated Running time: " << numOfRuns * runTimeLimit << "s\n";

		float runTotalDT = 0;
		float runHighestDT = 0;
		int runCycleCounter = 0;

		//std::vector<std::vector<std::vector<float>*>*> runDeltaTimes; //store every delta time for every run (particleNum.dividerNum.dT)
		std::vector<int> runParticleNums;
		std::vector<int> runDividerNums;
	
		std::vector<std::vector<float>> runAvgDeltatimes; //store avg delta time of every run;
		std::vector<std::vector<float>> runMaxDeltatimes; //store max delta time of every run;

		lastRunTimeMS = SDL_GetTicks();

		int pRunCounter = 0;
		int dRunCounter = 0;

		Uint32 cycleCounter;


		//update window title
		Uint32 lastTitleUpdateMS = SDL_GetTicks();
		Uint32 currentTitleUpdateTimeMS = 0;
		float titleUpdateDT = 0.0f;
		float titleUpdateTimeCounter = 0.0f;
		float titleUpdateDelay = 0.5f;

	
		for (int p = particleStartingNum; p <= particleFinishingNum; p += particleIncrementer)
		{
			runParticleNums.push_back(p);
			dRunCounter = 0;

			std::vector<float> tempDVec1;
			std::vector<float> tempDVec2;

			std::cout << "P = " << p ;

			for (int d = dividerStartingNum; d <= dividerFinishingNum; d += dividerIncrementer)
			{
				std::cout << "\t" << "D = " << d << "\n";

				if (p == particleStartingNum)
				{
					//store divider numbers

					runDividerNums.push_back(d);
				}

				totalRunTime = 0.0f;
				cycleCounter = 0;

				currentScene->Setdivider(d);
				currentScene->Restart(p);
				while (totalRunTime <= runTimeLimit)
				{
					currentRunTimeMS = SDL_GetTicks();
					runDeltaTime = (float)((currentRunTimeMS - lastRunTimeMS) / 1000.0f);
					lastRunTimeMS = currentRunTimeMS;

					totalRunTime += runDeltaTime;

					currentScene->ProcessFrame();


					currentTitleUpdateTimeMS = SDL_GetTicks();
					titleUpdateDT = (float)((currentTitleUpdateTimeMS - lastTitleUpdateMS) / 1000.0f);
					titleUpdateTimeCounter += titleUpdateDT;
					if (titleUpdateTimeCounter >= titleUpdateDelay)
					{
						currentScene->SetWindowTitle(window);
						titleUpdateTimeCounter = 0.0f;
					}
					lastTitleUpdateMS = currentTitleUpdateTimeMS;
					

					runTotalDT += currentScene->GetCurrDT();
					runCycleCounter++;
				}

				tempDVec1.push_back(runTotalDT / runCycleCounter);
				tempDVec2.push_back(currentScene->GetHighestDT());

				runTotalDT = 0.0f;
				runCycleCounter = 0;

				runDeltaTime = 0.0f;

				dRunCounter++;
			}

			runAvgDeltatimes.push_back(tempDVec1);
			runMaxDeltatimes.push_back(tempDVec2);

			pRunCounter++;
		}
	

		std::cout << "P\tD\tAdT\t\tMdT\n";

		for (int p = 0; p < runParticleNums.size(); p++)
		{
			for (int d = 0; d < runDividerNums.size(); d++)
			{
				std::cout << runParticleNums.at(p) << "\t" << runDividerNums.at(d) << "\t" << runAvgDeltatimes.at(p).at(d) << "\t" << runMaxDeltatimes.at(p).at(d) << "\n";
			}
		}

		bool breakout = false;

		currentScene->Setdivider(2);
		currentScene->Restart(2000);
		while (breakout == false)
		{
			currentScene->ProcessFrame();
			currentScene->SetWindowTitle(window);

			while (SDL_PollEvent(&ev) != 0)
			{
				if (ev.type == SDL_QUIT)
				{
					breakout = true;
				}
			}
		}
	}

	
	//cleanup
	delete currentScene;
	currentScene = nullptr;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}


	//debugging
	/*
	int ci = 2;
	int cj = 3000;

	int i = 1;
	int j = 1;

	currentScene->Setdivider(i++);
	currentScene->Restart(j += 1);

	currentScene->ProcessFrame();

	SDL_Delay(500);

	currentScene->Setdivider(i++);
	currentScene->Restart(j += 1);

	currentScene->ProcessFrame();

	SDL_Delay(500);

	currentScene->Setdivider(i++);
	currentScene->Restart(j += 1);

	currentScene->ProcessFrame();

	SDL_Delay(500);
	*/


		//SDL_Delay(FPS_DELAY);

	
	/*

	//testing hashing function

	mySpatialHashingTable* hashTest = new mySpatialHashingTable(900, 900);
	int currHash;

	

	for (int x = 0; x < hashTest->GetDivider(); x++)
	{
		for (int y = 0; y < hashTest->GetDivider(); y++)
		{
			//currHash = hashTest->HashForCellCoord(glm::ivec2(x, y));
		}
	}
	*/