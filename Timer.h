#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

class Timer
{
    std::thread thread;
    bool alive = false;
    long callNumber = Infinite;
    long repeatCount = Infinite;
    std::chrono::milliseconds interval = std::chrono::milliseconds(0);
    std::function<void(void)> funct = nullptr;

    void SleepAndRun()
    {
        std::this_thread::sleep_for(interval);
        if (IsAlive())
            GetFunction()();
    }

    void ThreadFunc()
    {
        if (callNumber == Infinite)
            while (IsAlive())
                SleepAndRun();
        else
            while (repeatCount--)
                SleepAndRun();
    }

public:
    static const long Infinite = -1L;

    Timer() {}
    Timer(const std::function<void(void)> &f) : funct(f) {}
    Timer(const std::function<void(void)> &f,
          const unsigned long &i,
          const unsigned long repeat = Timer::Infinite) :
              funct(f),
              interval(std::chrono::milliseconds(i)),
              callNumber(repeat) {}

    void Start(bool async = true)
    {
        if (IsAlive())
            return;

        alive = true;
        repeatCount = callNumber;

        if (async)
            thread = std::thread(&ThreadFunc, this);
        else
            this->ThreadFunc();
    }

    void Stop()
    {
        alive = false;
        thread.join();
    }

    void SetFunction(const std::function<void(void)> &f)
    {
        funct = f;
    }

    bool IsAlive() const { return alive; }

    void SetRepeatCount(const long r)
    {
        if (IsAlive())
            return;
        callNumber = r;
    }

    long GetLeftCount() const { return repeatCount; }

    long GetRepeatCount() const { return callNumber; }

    void SetInterval(const unsigned long &i)
    {
        if (IsAlive())
            return;
        interval = std::chrono::milliseconds(i);
    }

    unsigned long GetInterval() const { return interval.count(); }

    const std::function<void(void)> &GetFunction() const
    {
        return funct;
    }
};

#endif // TIMER_H

