#pragma once

#include <windows.h>
namespace Utility {

	class Timer {
	public:
		Timer() : {
			QueryPerformanceFrequency(&Frequency);
		}

		void Start() {
			QueryPerformanceFrequency(&mStart);
			QueryPerformanceFrequency(&mPrevT);
		}

		double GetDeltaTime() {
			QueryPerformanceFrequency(&mCurrT);
			LARGE_INTEGER ElapsedMicroseconds;
			ElapsedMicroseconds.QuadPart = mCurrT.QuadPart - mPrevT.QuadPart;
			mPrevT = mCurrT;
			//
			// We now have the elapsed number of ticks, along with the
			// number of ticks-per-second. We use these values
			// to convert to the number of elapsed microseconds.
			// To guard against loss-of-precision, we convert
			// to microseconds *before* dividing by ticks-per-second.
			//
			ElapsedMicroseconds.QuadPart *= 1000000;
			return ElapsedMicroseconds.QuadPart / Frequency.QuadPart;
		}


	private:
		LARGE_INTEGER Frequency;
		LARGE_INTEGER mStart;
		LARGE_INTEGER mPrevT;
		LARGE_INTEGER mCurrT;
	};

}