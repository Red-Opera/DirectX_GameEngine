#pragma once

class GameTimer
{
public:
	GameTimer();

	float TotalTime() const;	// �� ����
	float DeltaTime() const;	// �� ����

	void Reset();		// �޼��� ���� ������ ȣ���ؾ� ��
	void Start();		// Ÿ�Ӹ� ���ۨ��簳 �� ȣ���ؾ� ��
	void Stop();		// �Ͻ������� ȣ���ؾ� ��
	void Tick();		// �� �����Ӹ��� ȣ���ؾ� ��
	float CheckTime();	// ���� Ÿ�Ӹ� �ð� (ms)

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