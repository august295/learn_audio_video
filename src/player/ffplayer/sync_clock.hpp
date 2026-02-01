#pragma once

#include <chrono>
#include <limits>

class SyncClock
{
public:
    SyncClock()
    {
        InitClock();
    }
    ~SyncClock()
    {
    }

    void InitClock()
    {
        pts_       = std::numeric_limits<double>::quiet_NaN();
        pts_drift_ = 0.0;
    }

    void SetClockAt(double pts, double time)
    {
        pts_       = pts;
        pts_drift_ = pts - time;
    }

    void SetClock(double pts)
    {
        double time = GetTimeSeconds();
        SetClockAt(pts, time);
    }

    double GetClock()
    {
        double time = GetTimeSeconds();
        return pts_drift_ + time;
    }

private:
    double GetTimeSeconds() const
    {
        using namespace std::chrono;
        auto now      = high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return duration_cast<microseconds>(duration).count() / 1e6;
    }

private:
    double pts_       = 0.0;
    double pts_drift_ = 0.0;
};
