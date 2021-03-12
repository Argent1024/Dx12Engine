#pragma once

#include <windows.h>
namespace Utility {
	// TODO add second/microsecond parameters
	class Timer {
	public:
		Timer() {
			QueryPerformanceFrequency(&Frequency);
		}

		void Start() {
			QueryPerformanceCounter(&mStart);
			QueryPerformanceCounter(&mPrevT);
			mCurrT = mPrevT;
		}

		double GetDeltaTime() {
			QueryPerformanceCounter(&mCurrT);
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

		double GetCurrent() {
			return mCurrT.QuadPart * 1000000 / Frequency.QuadPart;
		}

	private:
		LARGE_INTEGER Frequency;
		LARGE_INTEGER mStart;
		LARGE_INTEGER mPrevT;
		LARGE_INTEGER mCurrT;
	};

}