#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include <vector>
#include <map>
#include <iostream>
#include <unordered_map>

#include <glm/vec2.hpp>

#include <bitset> //debugging

#include <SDL2\SDL.h>

#include "Particle.h"

class mySpatialHashingTable
{
public:
	mySpatialHashingTable();
	mySpatialHashingTable(int _height, int _width);
	~mySpatialHashingTable();

	int GetDivider();
	int GetNumOfCells();

	void ClearBuckets();
	void InitTableOfIndex(int _divider);

	//takes position, returns index for pushing particle to a bucket
	long HashForPos(glm::vec2 _pos, float _particleRad);
	long HashForCellCoord(glm::ivec2 _cellCord);

	void AddParticle(myParticle* _p);
	std::vector<myParticle*> GetBucketContents(glm::ivec2 _cellCoord);
	std::vector<myParticle*> GetBucketContents(long _key);

	void OutputContents();
	void DrawBuckets(SDL_Renderer* _renderer);

protected:
private:

	int m_totalHeight;
	int m_totalWidth;

	float m_cellHeight;
	float m_cellWidth;

	int m_divider;	 //x => (width/x,height/x) as cell size
					//e.g. 1 => (width/1,height/1) as cell size
	int m_divbitSize; //number of bits the divider takes up, used in the hash function
	int m_numberOfCells; // = m_divider^2

	//uses multimap for the hash table array 
		//uses vectors for buckets

	std::unordered_map<long, std::vector<myParticle*> > m_tableOfIndex;
};


#endif // !_HASH_TABLE_H_
