#pragma once
#include "defines.h"

class TimeInterval {
public:
    TimeInterval(TimePoint start, TimePoint end);

    TimePoint start() const;

    TimePoint end() const;
    Duration GetTimeSpan(TimePoint stamp = START_TIME_POINT) const;

    bool Intersects(const TimeInterval& other) const;
    bool operator<(const TimeInterval& other) const;

    bool operator==(const TimeInterval& other) const;
    bool operator!=(const TimeInterval& other) const;

private:
    TimePoint start_{START_TIME_POINT};
    TimePoint end_{START_TIME_POINT};
};

class NamedTimeInterval : public TimeInterval {
public:
    NamedTimeInterval(size_t operation_id, TimePoint start, TimePoint end);
    size_t operation_id() const;

private:
    size_t operation_id_;
};
