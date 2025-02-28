#include "stdafx.h"
#include "GameTimer.h"

// GameTimer �ʱ�ȭ
GameTimer::GameTimer() : secondsPerFrame(0.0), deltaTime(-1.0), totalTime(0), pauseTime(0),
						 prevTime(0), currentTime(0), isStopped(false), stopTime(0), beforeTimePoint(0)
{
	// �ʴ� �������� ������
	__int64 framePerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&framePerSec);

	secondsPerFrame = 1.0f / (double)framePerSec;
}

void GameTimer::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	totalTime = currentTime;
	prevTime = currentTime;		// ���� �������� �����Ƿ� ���� ���������� ����
	stopTime = 0;

	isStopped = false;
}

// �Ͻ������� �ð��� �ٽ� ����ϴ� �޼ҵ�
void GameTimer::Start()
{
	// �ߴ� ���°� �ƴ� ��� �ƹ��͵� ���� ����
	if (!this->isStopped)
		return;

	/*
		�ߴܰ� ����(�簳) ���̿� �帥 �ð��� �����Ѵ�.

							|<-------d---------->|
		--------------------*--------------------*--------------------> �ð�
						stopTime			 startTIme
	*/

	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// �Ͻ����� �ð��� ������
	this->pauseTime += (startTime - stopTime);

	this->prevTime = startTime;	// ���� �������� �ʱ�ȭ��

	// �Ͻ����� �ð� �ʱ�ȭ
	this->stopTime = 0;	
	this->isStopped = false;
}

// Ÿ�Ӹ��� �ð��� �Ͻ� �����ϱ� ���� �޼ҵ�
void GameTimer::Stop()
{
	// �̹� Ÿ�̸Ӱ� �����ִٸ� �ƹ��͵� ���� ����
	if (this->isStopped)
		return;

	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	// Ÿ�Ӹ��� �Ͻ� �����ð��� ���� �ð����� ���ϰ� ���� ���θ� true�� ����
	this->stopTime = currentTime;
	isStopped = true;
}

// ���� ������ �� �ð�(deltaTime)�� ����
void GameTimer::Tick()
{
	// ���� ������ ���� ��� 0.0f�� ����
	if (isStopped)
	{
		deltaTime = 0.0f;
		return;
	}

	// ���� �������� �ð��� ����
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	this->currentTime = currentTime;

	// ���� �����Ӱ� ���� �������� ���� �̿��Ͽ� ������ �� �ð��� ���Ѵ�.
	this->deltaTime = (currentTime - prevTime) * secondsPerFrame;

	this->prevTime = this->currentTime;

	// deltaTime�� ������ ���� �ʵ��� ����
	if (this->deltaTime < 0.0)
		this->deltaTime = 0.0;
}

float GameTimer::CheckTime()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	float totalTime = (float)((currentTime - beforeTimePoint) * secondsPerFrame);
	beforeTimePoint = currentTime;

	return totalTime;
}

float GameTimer::DeltaTime() const { return (float)deltaTime; }

float GameTimer::TotalTime() const
{
	/*
		���� Ÿ�̸Ӱ� �ߴ� �����̸� �ߴܵ� �ķκ��� �帥 �ð��� ������� �ʴ´�.
		���� ������ �Ͻ� ������ ���� �ִٸ� �� �ð��� ��ü �ð��� ���Խ�Ű�� �ʴ´�.
		�̸� ���� stopTime���� �Ͻ����� �ð��� �A��.

				   ������ �Ͻ����� �ð�
				    |<-----d------>|
		-*----------*--------------*---*-----------*----> �ð�
		totalTime					 stopTime	currentTime
	*/

	if (this->isStopped)
		return (float)(((this->stopTime - this->pauseTime) - totalTime) * secondsPerFrame);

	/*
		�ð��� currentTime - totalTime���� �Ͻ� ������ �ð��� ���ԵǾ� �ִ�.
		�̸� ��ü �ð��� ���Խ�Ű�� �� �ǹǷ�, �� �ð��� currentTime���� �����Ѵ�.

			(currentTime - pausedTime) - totalTime

				   ������ �Ͻ����� �ð�
					|<-----d------>|
		-*----------*--------------*-------------*----> �ð�
		totalTime  stopTime	    startTime	currentTime
	*/

	else
		return (float)(((currentTime - pauseTime) - totalTime) * secondsPerFrame);
}