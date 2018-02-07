#pragma once
#include <chrono>

namespace snes
{
	typedef std::chrono::high_resolution_clock Clock;
	constexpr uint64 MS_IN_S = 1000;
	constexpr uint64 NS_IN_MS = 1000000;
	constexpr uint64 NS_IN_S = 1000000000;

	/** FrameTime
	  * Platform-independent utility for setting and calculating the duration of a frame,
	  * as well as calculating how often to run a "fixed-time-step" loop. */
	class FrameTime
	{
	public:
		FrameTime();
		~FrameTime();

		/** The number of times FixedLogic() will be called on all components per second */
		static constexpr uint8 FIXED_LOGIC_LOOPS_PER_SECOND = 60;
		/** The time step between fixed logic loops */
		static constexpr float SECONDS_PER_FIXED_LOOP = 1.0f / FIXED_LOGIC_LOOPS_PER_SECOND;

		/** Marks the start of a new frame by doing the following:
		  * Sleeps until the maximum FPS is hit (optional)
		  * Calculates the duration of the frame
		  * Calculates the number of FixedLogic loops to run this frame */
		void StartNewFrame();

		/** @return the number of fixed logic loops to be completed this frame */
		uint GetPendingFixedLogicLoops() { return m_pendingFixedLogicLoops; }

		/** @return the duration of the last frame (in seconds) */
		static float GetLastFrameDuration() { return m_deltaTime; }

		/** Set the maximum allowable FPS */
		static void SetMaxFPS(uint maxFPS) { m_maxFPS = maxFPS; }
		static bool UseMaxFPS(bool set) { m_limitFPS = set; }

	private:
		static constexpr std::chrono::nanoseconds NS_PER_FIXED_LOGIC_LOOP{ NS_IN_S / FIXED_LOGIC_LOOPS_PER_SECOND };
		/** The time taken (in seconds) for the last frame to complete */
		static float m_deltaTime;
		/** The maximum FPS allowed */
		static uint m_maxFPS;
		/** Whether to limit the game to the max FPS */
		static bool m_limitFPS;

		/** The number of fixed logic loops to compete this frame */
		uint m_pendingFixedLogicLoops = 0;

		/** The time remaining until the next FixedLogic() call */
		std::chrono::nanoseconds m_fixedLogicTimeRemaining;
		/** The time that the application started running */
		std::chrono::steady_clock::time_point m_applicationStart;
		/** The time that the last frame began */
		std::chrono::steady_clock::time_point m_lastFrameStart;

	};
}
