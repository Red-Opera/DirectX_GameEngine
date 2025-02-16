#pragma once

#include "../GameTimer.h"

#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

class PerformanceLog
{
public:
	static void Start(const std::string& message = "") noexcept { get_().Start_(message); }
	static void CheckTime(const std::string& message = "") noexcept { get_().CheckTime_(message); }

private:
	struct TimePoint
	{
		TimePoint(std::string message, float time) : message(std::move(message)), time(time) { }

		void PrintTime(std::ostream& out) const noexcept
		{
			using namespace std::string_literals;

			if (message.empty())
				out << time * 1000.0f << "ms\n";

			else
				out << std::setw(16) << std::left << "["s << message << "] "s << 
				       std::setw(6) << std::right << time * 1000.0f << "ms\n";
		}

		std::string message;
		float time;
	};

	static PerformanceLog& get_() noexcept
	{
		static PerformanceLog performanceLog;

		return performanceLog;
	}

	PerformanceLog() noexcept { timePoints.reserve(3000); }
	~PerformanceLog() { flush(); }

	void Start_(const std::string& message = "") noexcept
	{
		timePoints.emplace_back(message, 0.0f);
		timer.CheckTime();
	}

	void CheckTime_(const std::string& message = "") noexcept
	{
		float time = timer.CheckTime();
		timePoints.emplace_back(message, time);
	}

	void flush()
	{
		std::ofstream file("PerformanceLog.txt");
		file << std::setprecision(3) << std::fixed;

		for (const auto& timePoint : timePoints)
			timePoint.PrintTime(file);
	}

	GameTimer timer;
	std::vector<TimePoint> timePoints;
};