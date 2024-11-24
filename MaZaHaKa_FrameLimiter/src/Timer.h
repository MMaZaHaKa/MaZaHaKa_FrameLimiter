#pragma once
#define FIX_BUGS
#include "Windows.h"
#include <iostream> // uint32_t

class CTimer
{

	static uint32_t m_snTimeInMilliseconds;
	static uint32_t m_snTimeInMillisecondsPauseMode;
	static uint32_t m_snTimeInMillisecondsNonClipped;
	static uint32_t m_snPreviousTimeInMilliseconds;
	static uint32_t m_FrameCounter;
	static float ms_fTimeScale;
	static float ms_fTimeStep;
	static float ms_fTimeStepNonClipped;
#ifdef FIX_BUGS
	static uint32_t m_LogicalFrameCounter;
	static uint32_t m_LogicalFramesPassed;
#endif
public:
	static bool  m_UserPause;
	static bool  m_CodePause;

	static const float &GetTimeStep(void) { return ms_fTimeStep; }
	static void SetTimeStep(float ts) { ms_fTimeStep = ts; }
	static float GetTimeStepInSeconds() { return ms_fTimeStep / 50.0f; }
	static uint32_t GetTimeStepInMilliseconds() { return ms_fTimeStep / 50.0f * 1000.0f; }
	static const float &GetTimeStepNonClipped(void) { return ms_fTimeStepNonClipped; }
	static float GetTimeStepNonClippedInSeconds(void) { return ms_fTimeStepNonClipped / 50.0f; }
	static float GetTimeStepNonClippedInMilliseconds(void) { return ms_fTimeStepNonClipped / 50.0f * 1000.0f; }
	static void SetTimeStepNonClipped(float ts) { ms_fTimeStepNonClipped = ts; }
	static const uint32_t &GetFrameCounter(void) { return m_FrameCounter; }
	static void SetFrameCounter(uint32_t fc) { m_FrameCounter = fc; }
	static const uint32_t &GetTimeInMilliseconds(void) { return m_snTimeInMilliseconds; }
	static void SetTimeInMilliseconds(uint32_t t) { m_snTimeInMilliseconds = t; }
	static uint32_t GetTimeInMillisecondsNonClipped(void) { return m_snTimeInMillisecondsNonClipped; }
	static void SetTimeInMillisecondsNonClipped(uint32_t t) { m_snTimeInMillisecondsNonClipped = t; }
	static uint32_t GetTimeInMillisecondsPauseMode(void) { return m_snTimeInMillisecondsPauseMode; }
	static void SetTimeInMillisecondsPauseMode(uint32_t t) { m_snTimeInMillisecondsPauseMode = t; }
	static uint32_t GetPreviousTimeInMilliseconds(void) { return m_snPreviousTimeInMilliseconds; }
	static void SetPreviousTimeInMilliseconds(uint32_t t) { m_snPreviousTimeInMilliseconds = t; }
	static const float &GetTimeScale(void) { return ms_fTimeScale; }
	static void SetTimeScale(float ts) { ms_fTimeScale = ts; }
	static uint32_t GetCyclesPerFrame();

	static bool GetIsPaused() { return m_UserPause || m_CodePause; }
	static bool GetIsUserPaused() { return m_UserPause; }
	static bool GetIsCodePaused() { return m_CodePause; }
	static void SetCodePause(bool pause) { m_CodePause = pause; }
	
	static void Initialise(void);
	static void Shutdown(void);
	static void Update(void);
	static void Suspend(void);
	static void Resume(void);
	static uint32_t GetCyclesPerMillisecond(void);
	static uint32_t GetCurrentTimeInCycles(void);
	static bool GetIsSlowMotionActive(void);
	static void Stop(void);
	static void StartUserPause(void);
	static void EndUserPause(void);

	friend bool GenericLoad(void);
	friend bool GenericSave(int file);
	friend class CMemoryCard;

#ifdef FIX_BUGS
	static float GetDefaultTimeStep(void) { return 50.0f / 30.0f; }
	static float GetTimeStepFix(void) { return GetTimeStep() / GetDefaultTimeStep(); }
	static uint32_t GetLogicalFrameCounter(void) { return m_LogicalFrameCounter; }
	static uint32_t GetLogicalFramesPassed(void) { return m_LogicalFramesPassed; }
#endif
};