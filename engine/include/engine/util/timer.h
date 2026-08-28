// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_UTIL_TIMER_H
#define UNNAMEDGAME_ENGINE_UTIL_TIMER_H

#include <chrono>
#include <thread>

namespace util {
    template<class C>
    class Timer {
    public:
        using Clock = C;

        Timer() = default;

        void start() {
            auto now = Clock::now();

            mStartTime = now;
            mLastTickTime = now;
            mLastLimitTime = now;

            mDeltaTime = 0.0f;
            mElapsedTime = 0.0;
            mRate = 0.0f;

            mRateAccumulator = 0.0;
            mRateFrameCount = 0;

            mStarted = true;
        }

        float tick() {
            auto now = Clock::now();

            mDeltaTime = std::chrono::duration<float>(now - mLastTickTime).count();
            mLastTickTime = now;

            mElapsedTime = std::chrono::duration<double>(now - mStartTime).count();

            mRateAccumulator += mDeltaTime;
            mRateFrameCount++;

            if (mRateAccumulator >= 1.0) {
                mRate = static_cast<float>(mRateFrameCount / mRateAccumulator);
                mRateAccumulator = 0.0;
                mRateFrameCount = 0;
            }

            return mDeltaTime;
        }

        float getDeltaTime() const { return mDeltaTime; }
        double getElapsedTime() const { return mElapsedTime; }
        float getRate() const { return mRate; }
        float getLimit() const { return mLimit; }
        void setLimit(float limit) { mLimit = std::max(limit, 0.0f); }

        void waitForLimit() {
            if (!mStarted || mLimit <= 0.0f) return;

            double targetTime = 1.0 / mLimit;
            double targetTime20P = targetTime * 0.2;

            while (true) {
                double elapsed = std::chrono::duration<double>(Clock::now() - mLastLimitTime).count();
                double remaining = targetTime - elapsed;

                if (remaining <= 0.0) break;

                if (remaining > targetTime20P) {
                    std::this_thread::sleep_for(std::chrono::duration<double>(remaining * 0.8));
                }
            }

            mLastLimitTime = Clock::now();
        }

    private:
        Clock::time_point mStartTime;
        Clock::time_point mLastTickTime;
        Clock::time_point mLastLimitTime;

        float mDeltaTime = 0.0f;
        double mElapsedTime = 0.0;

        float mRate = 0.0f;

        float mLimit = 0.0f;

        double mRateAccumulator = 0.0f;
        unsigned int mRateFrameCount = 0;

        bool mStarted = false;
    };
}

#endif //UNNAMEDGAME_ENGINE_UTIL_TIMER_H
