#include "stdafx.h"
#include "GameTimer.h"

// GameTimer 초기화
GameTimer::GameTimer() : secondsPerFrame(0.0), deltaTime(-1.0), totalTime(0), pauseTime(0),
						 prevTime(0), currentTime(0), isStopped(false), stopTime(0)
{
	// 초당 프레임을 설정함
	__int64 framePerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&framePerSec);

	secondsPerFrame = 1.0f / (double)framePerSec;
}

void GameTimer::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	totalTime = currentTime;
	prevTime = currentTime;		// 이전 프레임이 없으므로 현재 프레임으로 설정
	stopTime = 0;

	isStopped = false;
}

// 일시정지한 시간을 다시 재생하는 메소드
void GameTimer::Start()
{
	// 중단 상태가 아닌 경우 아무것도 하지 않음
	if (!this->isStopped)
		return;

	/*
		중단과 시작(재개) 사이에 흐른 시간을 누적한다.

							|<-------d---------->|
		--------------------*--------------------*--------------------> 시간
						stopTime			 startTIme
	*/

	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// 일시정지 시간을 누적함
	this->pauseTime += (startTime - stopTime);

	this->prevTime = startTime;	// 이전 프레임을 초기화함

	// 일시정시 시간 초기화
	this->stopTime = 0;	
	this->isStopped = false;
}

// 타임머의 시간을 일시 정지하기 위한 메소드
void GameTimer::Stop()
{
	// 이미 타이머가 멈춰있다면 아무것도 하지 않음
	if (this->isStopped)
		return;

	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	// 타임머의 일시 중지시간을 현재 시간으로 정하고 정지 여부를 true로 설정
	this->stopTime = currentTime;
	isStopped = true;
}

// 현재 프레임 당 시간(deltaTime)을 구함
void GameTimer::Tick()
{
	// 현재 게임을 멈출 경우 0.0f로 설정
	if (isStopped)
	{
		deltaTime = 0.0f;
		return;
	}

	// 현재 프레임의 시간을 구함
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	this->currentTime = currentTime;

	// 현재 프레임과 이전 프레임의 차를 이용하여 프레임 당 시간을 구한다.
	this->deltaTime = (currentTime - prevTime) * secondsPerFrame;

	this->prevTime = this->currentTime;

	// deltaTime이 음수가 되지 않도록 설정
	if (this->deltaTime < 0.0)
		this->deltaTime = 0.0;
}

float GameTimer::DeltaTime() const { return (float)deltaTime; }

float GameTimer::TotalTime() const
{
	/*
		현재 타이머가 중단 상태이면 중단된 후로부터 흐른 시간은 계산하지 않는다.
		또한 이전에 일시 정지된 적이 있다면 그 시간도 전체 시간에 포함시키지 않는다.
		이를 위해 stopTime에서 일시정지 시간을 뺸다.

				   이전의 일시정지 시간
				    |<-----d------>|
		-*----------*--------------*---*-----------*----> 시간
		totalTime					 stopTime	currentTime
	*/

	if (this->isStopped)
		return (float)(((this->stopTime - this->pauseTime) - totalTime) * secondsPerFrame);

	/*
		시간차 currentTime - totalTime에는 일시 정지된 시간이 포함되어 있다.
		이를 전체 시간에 포함시키면 안 되므로, 그 시간을 currentTime에서 제거한다.

			(currentTime - pausedTime) - totalTime

				   이전의 일시정지 시간
					|<-----d------>|
		-*----------*--------------*-------------*----> 시간
		totalTime  stopTime	    startTime	currentTime
	*/

	else
		return (float)(((currentTime - pauseTime) - totalTime) * secondsPerFrame);
}