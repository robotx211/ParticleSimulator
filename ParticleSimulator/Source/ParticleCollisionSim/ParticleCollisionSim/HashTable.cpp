#include "HashTable.h"

mySpatialHashingTable::mySpatialHashingTable()
{
	//assumed 1280/720

	m_totalHeight = 720;
	m_totalWidth = 1280;

	m_divider = 5;
	m_numberOfCells = m_divider * m_divider;

	float temp = m_divider - 1; //eg if divider = 4, uses numbers 0,1,2,3 - which are 2 bits
	m_divbitSize = 1; //must be at least 1 bit (for 0 or 1);
	while (temp > 1)
	{
		temp = temp / 2; 
		m_divbitSize++;
	}

	m_cellHeight = (float) m_totalHeight / (float) m_divider;
	m_cellWidth = (float) m_totalWidth / (float) m_divider;

	glm::ivec2 cellCoord;

	for (int x = 0; x < m_divider; x++)
	{
		for (int y = 0; y < m_divider; y++)
		{
			cellCoord = glm::ivec2(x, y);
			m_tableOfIndex.insert(std::pair<long, std::vector<myParticle*> >(HashForCellCoord(cellCoord), std::vector<myParticle*>()));
		}
	}

	//add -1 key cell to handle particles outside window, do not calc collisions on these
	m_tableOfIndex.insert(std::pair<long, std::vector<myParticle*> >(-1, std::vector<myParticle*>()));

}

mySpatialHashingTable::mySpatialHashingTable(int _height, int _width)
{
	m_totalHeight = _height;
	m_totalWidth = _width;

	m_divider = 5;
	m_numberOfCells = m_divider * m_divider;

	float temp = m_divider - 1;
	m_divbitSize = 1; //must be at least 1 bit (for 0 or 1);
	while (temp > 1)
	{
		temp = temp / 2;
		m_divbitSize++;
	}

	m_cellHeight = (float)m_totalHeight / (float)m_divider;
	m_cellWidth = (float)m_totalWidth / (float)m_divider;

	glm::ivec2 cellCoord;

	for (int x = 0; x < m_divider; x++)
	{
		for (int y = 0; y < m_divider; y++)
		{
			cellCoord = glm::ivec2(x, y);
			m_tableOfIndex.insert(std::pair<long, std::vector<myParticle*> >(HashForCellCoord(cellCoord), std::vector<myParticle*>()));
		}
	}
	m_tableOfIndex.insert(std::pair<long, std::vector<myParticle*> >(-1, std::vector<myParticle*>()));
}

mySpatialHashingTable::~mySpatialHashingTable()
{
	ClearBuckets();
}


//-----------------------------------------------------------------------------------------------------------------//


int mySpatialHashingTable::GetDivider()
{
	return m_divider;
}

int mySpatialHashingTable::GetNumOfCells()
{
	return m_numberOfCells;
}


//-----------------------------------------------------------------------------------------------------------------//


void mySpatialHashingTable::ClearBuckets()
{
	std::unordered_map<long, std::vector<myParticle*>>::iterator it;

	for (it = m_tableOfIndex.begin(); it != m_tableOfIndex.end(); ++it)
	{
		if (it == m_tableOfIndex.end())
		{
			std::cout << "END ITERATOR: HashTable.cpp, Line 103!" << std::endl;
		}

		(*it).second.clear();
	}
}

void mySpatialHashingTable::InitTableOfIndex(int _divider)
{
	ClearBuckets();
	m_tableOfIndex.clear();

	glm::ivec2 cellCoord;

	m_divider = _divider;
	m_numberOfCells = m_divider * m_divider;

	m_cellHeight = (float)m_totalHeight / (float)m_divider;
	m_cellWidth = (float)m_totalWidth / (float)m_divider;

	float temp = m_divider - 1; //cells are numbered (0,0) to (D-1,D-1)
	m_divbitSize = 1; //must be at least 1 bit (for 0 or 1);
	while (temp > 1)
	{
		temp = temp / 2;
		m_divbitSize++;
	}

	m_tableOfIndex.clear();

	for (int x = 0; x < m_divider; x++)
	{
		for (int y = 0; y < m_divider; y++)
		{
			cellCoord = glm::ivec2(x, y);
			m_tableOfIndex.insert(std::pair<long, std::vector<myParticle*> >(HashForCellCoord(cellCoord), std::vector<myParticle*>()));
		}
	}

	//extra cell used for processing particles outside the screen
	m_tableOfIndex.insert(std::pair<long, std::vector<myParticle*> >(-1, std::vector<myParticle*>()));
}


//-----------------------------------------------------------------------------------------------------------------//


long mySpatialHashingTable::HashForPos(glm::vec2 _pos, float _particleRad)
{
	this;

	float tempX = _pos.x;
	float tempY = _pos.y;

	if (tempX - _particleRad < 0 || tempX + _particleRad >= m_totalWidth || tempY - _particleRad < 0 || tempY + _particleRad >= m_totalHeight)
	{
		//returns error value, so particle is not put in a bucket
		//therefore does not have collisions calculated on it
		return -1;
	}

	int cellX = 0;
	int cellY = 0;

	while (tempX > m_cellWidth)
	{
		tempX -= m_cellWidth;
		cellX++;
	}
	while (tempY > m_cellHeight)
	{
		tempY -= m_cellHeight;
		cellY++;
	}

	for (int i = 0; i < m_divbitSize; i++)
	{
		cellX <<= 0x00001;
	}

	long index = cellX + cellY;

	return index;
}

long mySpatialHashingTable::HashForCellCoord(glm::ivec2 _cellCord)
{
	long cellX = _cellCord.x;
	//std::cout << "(" << _cellCord.x << "," << _cellCord.y << ")\n";

	for (long i = 0; i < m_divbitSize; i++)
	{
		//std::cout << i << ": " << std::bitset<8>(cellX) << "\t";
		cellX <<= 0x00001;
	}

	//std::cout << m_divbitSize << "(FINAL): " << std::bitset<8>(cellX) << "\n";

	long index = cellX + _cellCord.y;

	//std::cout << "HASH: " << index << "\n\n";

	return index;
}


//-----------------------------------------------------------------------------------------------------------------//


void mySpatialHashingTable::AddParticle(myParticle* _p)
{
	long hashKeyTL = HashForPos(_p->m_position + glm::vec2( (-1) * _p->m_radius, (-1) * _p->m_radius), _p->m_radius); //top left corner hash key, - rax in x, - rad in y
	long hashKeyTR = HashForPos(_p->m_position + glm::vec2( ( 1) * _p->m_radius, (-1) * _p->m_radius), _p->m_radius); //top right corner hash key, + rax in x, - rad in y
	long hashKeyBL = HashForPos(_p->m_position + glm::vec2( (-1) * _p->m_radius, ( 1) * _p->m_radius), _p->m_radius); //bot left corner hash key, - rax in x, + rad in y
	long hashKeyBR = HashForPos(_p->m_position + glm::vec2( ( 1) * _p->m_radius, ( 1) * _p->m_radius), _p->m_radius); //bot right corner hash key, + rax in x, + rad in y

	auto tempIteratorTL = m_tableOfIndex.find(hashKeyTL);
	auto tempIteratorTR = m_tableOfIndex.find(hashKeyTR);
	auto tempIteratorBL = m_tableOfIndex.find(hashKeyBL);
	auto tempIteratorBR = m_tableOfIndex.find(hashKeyBR);

	if (tempIteratorTL == m_tableOfIndex.end())
	{
		std::cout << "END ITERATOR - AddParticle!" << std::endl;
	}
	else
	{
		tempIteratorTL->second.push_back(_p);
	}

	if (hashKeyTR != hashKeyTL)
	{
		if (tempIteratorTR == m_tableOfIndex.end())
		{
			std::cout << "END ITERATOR - AddParticle!" << std::endl;
		}
		else
		{
			tempIteratorTR->second.push_back(_p);
		}
	}

	if (hashKeyBL != hashKeyTL && hashKeyBL != hashKeyTR)
	{
		if (tempIteratorBL == m_tableOfIndex.end())
		{
			std::cout << "END ITERATOR - AddParticle!" << std::endl;
		}
		else
		{
			tempIteratorBL->second.push_back(_p);
		}
	}

	if (hashKeyBR != hashKeyTL && hashKeyBR != hashKeyTR && hashKeyBR != hashKeyBL)
	{
		if (tempIteratorBR == m_tableOfIndex.end())
		{
			std::cout << "END ITERATOR - AddParticle!" << std::endl;
		}
		else
		{
			tempIteratorBR->second.push_back(_p);
		}
	}
}

std::vector<myParticle*> mySpatialHashingTable::GetBucketContents(glm::ivec2 _cellCoord)
{
	return m_tableOfIndex.find(HashForCellCoord(_cellCoord))->second;

	auto temp = m_tableOfIndex.find(HashForCellCoord(_cellCoord));
	if (temp == m_tableOfIndex.end())
	{
		std::cout << "END ITERATOR - GetBucketContents!" << std::endl;
		std::vector<myParticle*> null;
		return null;
	}
	else
	{
		return temp->second;
	}
}

std::vector<myParticle*> mySpatialHashingTable::GetBucketContents(long _key)
{
	return m_tableOfIndex.find(_key)->second;
}


//-----------------------------------------------------------------------------------------------------------------//


void mySpatialHashingTable::OutputContents()
{
	//use iterators to output all cells in the multimap
	std::unordered_map<long, std::vector<myParticle*>>::iterator it;
	int counter = 1;

	for (it = m_tableOfIndex.begin(); it != m_tableOfIndex.end(); ++it)
	{
		if (it == m_tableOfIndex.end())
		{
			std::cout << "END ITERATOR: HashTable.cpp, Line 239!" << std::endl;
		}

		std::cout << "Bucket " << counter << "\n\tKey: " << (*it).first << ", Contents: " << (*it).second.size() << " particles\n";
		counter++;
	}
}

void mySpatialHashingTable::DrawBuckets(SDL_Renderer* _renderer)
{
	glm::vec2 topLeft, topRight, botLeft, botRight;
	SDL_SetRenderDrawColor(_renderer, 0, 255, 0, 0);
	for (int x = 0; x < m_divider; x++) //x
	{
		for (int y = 0; y < m_divider; y++) //y
		{
			topLeft.x = m_cellWidth * x;
			topLeft.y = m_cellHeight * y;

			topRight.x = m_cellWidth * (x + 1);
			topRight.y = m_cellHeight * y;

			botLeft.x = m_cellWidth * x;
			botLeft.y = m_cellHeight * (y + 1);

			botRight.x = m_cellWidth * (x + 1);
			botRight.y = m_cellHeight * (y + 1);

			//only necessary to draw 2 sides
			//SDL_RenderDrawLine(_renderer, topLeft.x, topLeft.y, botLeft.x, botLeft.y);
			//SDL_RenderDrawLine(_renderer, topLeft.x, topLeft.y, topRight.x, topRight.y);
			SDL_RenderDrawLine(_renderer, botLeft.x, botLeft.y, botRight.x, botRight.y);
			SDL_RenderDrawLine(_renderer, topRight.x, topRight.y, botRight.x, botRight.y);

			//write cell number and number of particles in top left and right hand sides of the cell respectively
		}

	}
}