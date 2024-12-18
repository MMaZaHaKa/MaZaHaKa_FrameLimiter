//#define WITHWINDOWS
#define FIX_BUGS
//#define debug printf
#include "Timer.h"
#pragma comment(lib, "winmm.lib") // timeGetTime etc...


uint32_t CTimer::m_snTimeInMilliseconds;
uint32_t CTimer::m_snTimeInMillisecondsPauseMode = 1;
uint32_t CTimer::m_snTimeInMillisecondsNonClipped;
uint32_t CTimer::m_snPreviousTimeInMilliseconds;
uint32_t CTimer::m_FrameCounter;
float CTimer::ms_fTimeScale;
float CTimer::ms_fTimeStep;
float CTimer::ms_fTimeStepNonClipped;
bool  CTimer::m_UserPause;
bool  CTimer::m_CodePause;

#ifdef FIX_BUGS
uint32_t CTimer::m_LogicalFrameCounter;
uint32_t CTimer::m_LogicalFramesPassed;
#endif

uint32_t _nCyclesPerMS = 1;

#ifdef _WIN32
LARGE_INTEGER _oldPerfCounter;
LARGE_INTEGER perfSuspendCounter;
#define RsTimerType uint32_t
#else
#define RsTimerType double
#endif

RsTimerType oldPcTimer;

RsTimerType suspendPcTimer;

uint32_t suspendDepth;


#ifdef _WIN32 // skeleton
uint32_t
#else
double
#endif
RsTimer(void)
{
	uint32_t time;

	TIMECAPS TimeCaps;

	timeGetDevCaps(&TimeCaps, sizeof(TIMECAPS));

	timeBeginPeriod(TimeCaps.wPeriodMin);

	time = (uint32_t)timeGetTime();

	timeEndPeriod(TimeCaps.wPeriodMin);

	return time;
}


void CTimer::Initialise(void)
{
	//debug("Initialising CTimer...\n");
	
	ms_fTimeScale = 1.0f;
	ms_fTimeStep = 1.0f;
	suspendDepth = 0;
	m_UserPause = false;
	m_CodePause = false;

	m_snTimeInMillisecondsNonClipped = 0;
	m_snPreviousTimeInMilliseconds = 0;
	m_snTimeInMilliseconds = 1;
#ifdef FIX_BUGS
	m_LogicalFrameCounter = 0;
	m_LogicalFramesPassed = 0;
#endif
	
#ifdef _WIN32
	LARGE_INTEGER perfFreq;
	if ( QueryPerformanceFrequency(&perfFreq) )
	{
		OutputDebugString("Performance counter available\n");
		_nCyclesPerMS = uint32_t(perfFreq.QuadPart / 1000);
		QueryPerformanceCounter(&_oldPerfCounter);
	}
	else
#endif
	{
		OutputDebugString("Performance counter not available, using millesecond timer\n");
		_nCyclesPerMS = 0;
		oldPcTimer = RsTimer();
	}
	
	m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds;
	
	m_FrameCounter = 0;
	
	//debug("CTimer ready\n");
}

void CTimer::Shutdown(void)
{
	;
}

#ifdef FIX_BUGS
void CTimer::Update(void)
{
	static double frameTimeLogical = 0.0;
	static double frameTimeFraction = 0.0;
	static double frameTimeFractionScaled = 0.0;
	double frameTime;
	double dblUpdInMs;

	m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
	
#ifdef _WIN32
	if ( (double)_nCyclesPerMS != 0.0 )
	{
		LARGE_INTEGER pc;
		QueryPerformanceCounter(&pc);
		
		int32_t updInCycles = (pc.LowPart - _oldPerfCounter.LowPart); // & 0x7FFFFFFF; pointless
		
		_oldPerfCounter = pc;

		// bugfix from VC
		double updInCyclesScaled = GetIsPaused() ? updInCycles : updInCycles * ms_fTimeScale;

		frameTime = updInCyclesScaled / (double)_nCyclesPerMS;

		dblUpdInMs = (double)updInCycles / (double)_nCyclesPerMS;
	}
	else
#endif
	{
		RsTimerType timer = RsTimer();
		
		RsTimerType updInMs = timer - oldPcTimer;

		// bugfix from VC
		frameTime = GetIsPaused() ? (double)updInMs : (double)updInMs * ms_fTimeScale;

		oldPcTimer = timer;

		dblUpdInMs = (double)updInMs;
	}

	// count frames as if we're running at 30 fps
	m_LogicalFramesPassed = 0;
	frameTimeLogical += dblUpdInMs;
	while(frameTimeLogical >= 1000.0 / 30.0) {
		frameTimeLogical -= 1000.0 / 30.0;
		m_LogicalFramesPassed++;
	}
	m_LogicalFrameCounter += m_LogicalFramesPassed;

	frameTimeFraction += dblUpdInMs;
	frameTimeFractionScaled += frameTime;

	m_snTimeInMillisecondsPauseMode += uint32_t(frameTimeFraction);
															 
	if ( GetIsPaused() )
		ms_fTimeStep = 0.0f;

	else
	{
		m_snTimeInMilliseconds += uint32_t(frameTimeFractionScaled);
		m_snTimeInMillisecondsNonClipped += uint32_t(frameTimeFractionScaled);
		ms_fTimeStep = frameTime / 1000.0f * 50.0f;
	}
	frameTimeFraction -= uint32_t(frameTimeFraction);
	frameTimeFractionScaled -= uint32_t(frameTimeFractionScaled);
	
	if ( ms_fTimeStep < 0.01f && !GetIsPaused() )
		ms_fTimeStep = 0.01f;

	ms_fTimeStepNonClipped = ms_fTimeStep;
	
	//if ( !CRecordDataForGame::IsPlayingBack() )
	//{
	//	ms_fTimeStep = Min(3.0f, ms_fTimeStep);

	//	if ( (m_snTimeInMilliseconds - m_snPreviousTimeInMilliseconds) > 60 )
	//		m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 60;
	//}
 // 
	//if ( CRecordDataForChase::IsRecording() )
	//{
	//	ms_fTimeStep = 1.0f;
	//	m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 16;
	//}
 // 
	m_FrameCounter++;
}
#else
void CTimer::Update(void)
{
	m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
	
#ifdef _WIN32
	if ( (double)_nCyclesPerMS != 0.0 )
	{
		LARGE_INTEGER pc;
		QueryPerformanceCounter(&pc);
		
		int32_t updInCycles = (pc.LowPart - _oldPerfCounter.LowPart); // & 0x7FFFFFFF; pointless
		
		_oldPerfCounter = pc;

		float updInCyclesScaled = updInCycles * ms_fTimeScale;

		double frameTime = updInCyclesScaled / (double)_nCyclesPerMS;
		m_snTimeInMillisecondsPauseMode = m_snTimeInMillisecondsPauseMode + frameTime;
		
		if ( GetIsPaused() )
			ms_fTimeStep = 0.0f;
		else
		{
			m_snTimeInMilliseconds = m_snTimeInMilliseconds + frameTime;
			m_snTimeInMillisecondsNonClipped = m_snTimeInMillisecondsNonClipped + frameTime;
			ms_fTimeStep = frameTime / 1000.0f * 50.0f;
		}
	}
	else
#endif
	{
		RsTimerType timer = RsTimer();
		
		RsTimerType updInMs = timer - oldPcTimer;
		
		double frameTime = (double)updInMs * ms_fTimeScale;

		oldPcTimer = timer;
		
		m_snTimeInMillisecondsPauseMode = m_snTimeInMillisecondsPauseMode + frameTime;
															 
		if ( GetIsPaused() )
			ms_fTimeStep = 0.0f;
		else
		{
			m_snTimeInMilliseconds = m_snTimeInMilliseconds + frameTime;
			m_snTimeInMillisecondsNonClipped = m_snTimeInMillisecondsNonClipped + frameTime;
			ms_fTimeStep = frameTime / 1000.0f * 50.0f;
		}
	}
	
	if ( ms_fTimeStep < 0.01f && !GetIsPaused() )
		ms_fTimeStep = 0.01f;

	ms_fTimeStepNonClipped = ms_fTimeStep;
	
	if ( !CRecordDataForGame::IsPlayingBack() )
	{
		ms_fTimeStep = Min(3.0f, ms_fTimeStep);

		if ( (m_snTimeInMilliseconds - m_snPreviousTimeInMilliseconds) > 60 )
			m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 60;
	}
  
	if ( CRecordDataForChase::IsRecording() )
	{
		ms_fTimeStep = 1.0f;
		m_snTimeInMilliseconds = m_snPreviousTimeInMilliseconds + 16;
	}
  
	m_FrameCounter++;
}
#endif

void CTimer::Suspend(void)
{
	if ( ++suspendDepth > 1 )
		return;
	
#ifdef _WIN32
	if ( (double)_nCyclesPerMS != 0.0 )
		QueryPerformanceCounter(&perfSuspendCounter);
	else
#endif
		suspendPcTimer = RsTimer();
}

void CTimer::Resume(void)
{
	if ( --suspendDepth != 0 )
		return;

#ifdef _WIN32
	if ( (double)_nCyclesPerMS != 0.0 )
	{
		LARGE_INTEGER pc;
		QueryPerformanceCounter(&pc);

		_oldPerfCounter.LowPart += pc.LowPart - perfSuspendCounter.LowPart;
	}
	else
#endif
		oldPcTimer += RsTimer() - suspendPcTimer;
}

uint32_t CTimer::GetCyclesPerMillisecond(void)
{
#ifdef _WIN32
	if (_nCyclesPerMS != 0)
		return _nCyclesPerMS;
	else 
#endif
		return 1;
}

uint32_t CTimer::GetCurrentTimeInCycles(void)
{
#ifdef _WIN32
	if ( _nCyclesPerMS != 0 )
	{
		LARGE_INTEGER pc;
		QueryPerformanceCounter(&pc);
		return (pc.LowPart - _oldPerfCounter.LowPart); // & 0x7FFFFFFF; pointless
	}
	else
#endif
		return RsTimer() - oldPcTimer;
}

bool CTimer::GetIsSlowMotionActive(void)
{
	return ms_fTimeScale < 1.0f;
}

void CTimer::Stop(void)
{
	m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
}

void CTimer::StartUserPause(void)
{
	m_UserPause = true;
}

void CTimer::EndUserPause(void)
{
	m_UserPause = false;
}

uint32_t CTimer::GetCyclesPerFrame()
{
	return 20;
}

