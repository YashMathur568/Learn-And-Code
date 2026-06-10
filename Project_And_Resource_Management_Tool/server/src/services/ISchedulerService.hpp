#pragma once

class ISchedulerService {
public:
    virtual ~ISchedulerService() = default;

    virtual void start()   = 0;
    virtual void stop()    = 0;
    virtual void runOnce() = 0;
};
