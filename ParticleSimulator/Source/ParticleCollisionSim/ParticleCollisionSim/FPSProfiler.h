#ifndef _FPS_MANAGER_H_
#define _FPS_MANAGER_H_

#include <SDL2\SDL.h>
#include <vector>

class FPSProfiler
{

public:
	FPSProfiler();
	FPSProfiler(int _framePerFPSCalc, float _desiredFPS, bool _throttleFPS);

	void Reset();

	float GetHighestFrameDT() { return m_highestDT; }
	float GetAvgFrameDT() { return m_averageDT; }

	float GetFPS() { return m_framesPerSecond; }
	float GetDesiredFPS() { return m_desiredFPS; }

	bool IsDTHistoryFull() { return (m_DTHistory.size() == m_framesPerFPS); }

	void AddDT(float _DT);

	void RunFPSCalc();

	float SumOfDTHistory();

	void ToggleThrottleFPS() { m_throttleFPS = !m_throttleFPS; }

	bool GetThrottleFPSState() { return m_throttleFPS; }

protected:
private:

	//measured in seconds
	float m_currentDT;
	std::vector<float> m_DTHistory;
	float m_averageDT;
	float m_highestDT;

	int m_framesPerFPS; //number of DTs used in calculating an FPs, lower = more accurate FPS, higher = smother FPS counter (also decided how many DTs are stored in history)
	float m_framesPerSecond;
	float m_desiredFPS; //can be used to calc desired DT, used in throttling
	float m_desiredDT;

	bool m_throttleFPS; //force the system to wait so the FPS stays around the desired FPS

};

#endif // !_FPS_MANAGER_H_
