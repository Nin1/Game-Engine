#include "stdafx.h"
#include "FrameTime.h"
#include <thread>
#include <iostream>

namespace snes
{
	float FrameTime::m_deltaTime = 0.0f;
	uint FrameTime::m_maxFPS = 60;
	bool FrameTime::m_limitFPS = true;

	FrameTime::FrameTime()
	{
		m_applicationStart = Clock::now();
		m_lastFrameStart = m_applicationStart;
	}

	FrameTime::~FrameTime()
	{
	}

	void FrameTime::StartNewFrame()
	{
		auto thisFrameStart = Clock::now();
		std::chrono::duration<uint64, std::nano> frameDuration = thisFrameStart - m_lastFrameStart;
		
		// If the last frame was faster than our max FPS allows, wait until the next frame is due
		if (m_limitFPS)
		{
			std::chrono::duration<uint64, std::nano> minFrameDuration(NS_IN_S / m_maxFPS);
			if (frameDuration < minFrameDuration)
			{
				// @TODO: sleep_for() isn't very precise
				uint64 timeToWaitNS = minFrameDuration.count() - frameDuration.count();
				std::this_thread::sleep_for(std::chrono::nanoseconds(timeToWaitNS));
				thisFrameStart = Clock::now();

				// spin-locking is grooooosssss but is more accurate
				/*bool sleep = true;
				while (sleep)
				{
					std::this_thread::yield();
					thisFrameStart = Clock::now();
					if (thisFrameStart > m_lastFrameStart + minFrameDuration)
					{
						sleep = false;
					}
				}*/

				frameDuration = thisFrameStart - m_lastFrameStart;
			}
		}

		m_lastFrameStart = thisFrameStart;

		// Calculate the duration of the last frame
		m_deltaTime = (float)frameDuration.count() / NS_IN_S;

		// Calculate the number of fixed logic loops required tis frame to stay at the desired rate
		m_fixedLogicTimeRemaining += frameDuration;
		m_pendingFixedLogicLoops = 0;
		while (m_fixedLogicTimeRemaining > NS_PER_FIXED_LOGIC_LOOP)
		{
			m_fixedLogicTimeRemaining -= NS_PER_FIXED_LOGIC_LOOP;
			++m_pendingFixedLogicLoops;
		}
	}
}
