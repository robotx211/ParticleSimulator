#include "FPSProfiler.h"

FPSProfiler::FPSProfiler()
{
	m_currentDT = 0.0f;
	m_averageDT = 0.0f;
	m_highestDT = 0.0f;

	m_framesPerFPS = 60;
	m_framesPerSecond = 0.0f;
	m_desiredFPS = 60.0f;
	m_desiredDT = 1.0f / m_desiredFPS;

	m_throttleFPS = false;
}

FPSProfiler::FPSProfiler(int _framePerFPSCalc, float _desiredFPS, bool _throttleFPS)
{
	m_currentDT = 0.0f;
	m_averageDT = 0.0f;
	m_highestDT = 0.0f;

	m_framesPerFPS = _framePerFPSCalc;
	m_framesPerSecond = 0.0f;
	m_desiredFPS = _desiredFPS;
	m_desiredDT = 1.0f / m_desiredFPS;

	m_throttleFPS = _throttleFPS;
}


//-----------------------------------------------------------------------------------------------------------------//


void FPSProfiler::Reset()
{
	m_currentDT = 0.0f;
	m_averageDT = 0.0f;
	m_highestDT = 0.0f;

	m_framesPerSecond = 0.0f;
}


//-----------------------------------------------------------------------------------------------------------------//


void FPSProfiler::AddDT(float _DT)
{
	m_currentDT = _DT;

	if (m_DTHistory.size() < m_framesPerFPS)
	{
		m_DTHistory.push_back(_DT);
	}
	else
	{
		m_DTHistory.erase(m_DTHistory.begin());
		m_DTHistory.push_back(_DT);
	}

	if (_DT > m_highestDT)
	{
		m_highestDT = _DT;
	}
}


//-----------------------------------------------------------------------------------------------------------------//


void FPSProfiler::RunFPSCalc()
{
	m_averageDT = (SumOfDTHistory() / (float)m_DTHistory.size());

	m_framesPerSecond = 1.0f / m_averageDT;

	if (m_throttleFPS == true)
	{
		if (m_currentDT < m_desiredDT)
		{
			Uint32 delay = (Uint32)( (m_desiredDT - m_currentDT) * 1000);
			SDL_Delay(delay);
		}
	}
}


//-----------------------------------------------------------------------------------------------------------------//


float FPSProfiler::SumOfDTHistory()
{
	float total = 0.0f;

	for (int i = 0; i < m_DTHistory.size(); i++)
	{
		total += m_DTHistory.at(i);
	}

	return total;
}



