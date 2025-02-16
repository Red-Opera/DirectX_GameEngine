#pragma once

class GameTimer
{
public:
	GameTimer();

	float TotalTime() const;	// 초 단위
	float DeltaTime() const;	// 초 단위

	void Reset();		// 메세지 루프 이전에 호출해야 함
	void Start();		// 타임머 시작º재개 시 호출해야 함
	void Stop();		// 일시정지시 호출해야 함
	void Tick();		// 매 프레임마다 호출해야 함
	float CheckTime();	// 이전 타임머 시간 (ms)

private:
	double secondsPerFrame;
	double deltaTime;

	__int64 totalTime;
	__int64 pauseTime;
	__int64 stopTime;
	__int64 prevTime;
	__int64 currentTime;
	__int64 beforeTimePoint;

	bool isStopped;
};