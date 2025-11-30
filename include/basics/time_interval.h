#pragma once
#include "defines.h"

class TimeInterval {
public:
    TimeInterval(TimePoint start, TimePoint end);
    TimePoint Start() const;
    TimePoint End() const;
    Duration GetTimeSpan(TimePoint stamp = kStartTimePoint) const;

    bool Intersects(const TimeInterval& other) const;
    bool operator<(const TimeInterval& other) const;

    bool operator==(const TimeInterval& other) const;
    bool operator!=(const TimeInterval& other) const;

private:
    TimePoint start_{kStartTimePoint};
    TimePoint end_{kStartTimePoint};
};

class NamedTimeInterval : public TimeInterval {
public:
    NamedTimeInterval(size_t operation_id, TimePoint start, TimePoint end);
    size_t OperationID() const;

private:
    size_t operation_id_;
};
