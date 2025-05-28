#pragma once
#include "defines.h"
#include "operation.h"

class TimeInterval {
public:
    TimeInterval(TimePoint start, TimePoint end) : start_(start), end_(end) {}

    TimePoint start() const {
        return start_;
    }

    TimePoint end() const {
        return end_;
    }

    Duration GetTimeSpan(TimePoint stamp = START_TIME_POINT) const {
        if (stamp == START_TIME_POINT) {
            return end_ - start_;
        } 

        if (end_ > stamp) {
            return end_ - stamp;
        }

        return Duration(0);
    }

    bool Intersects(const TimeInterval& other) const {
        return !(end_ <= other.start_ || other.end_ < start_);
    }

    bool operator<(const TimeInterval& other) const {
        return start_ < other.start_;
    }

    bool operator==(const TimeInterval& other) const {
        return start_ <= other.start_ && other.end_ <= end_;
    }

    bool operator!=(const TimeInterval& other) const {
        return !(this->operator==(other));
    }

private:
    TimePoint start_{START_TIME_POINT};
    TimePoint end_{START_TIME_POINT};
};

class NamedTimeInterval : public TimeInterval {
public:
    NamedTimeInterval(size_t operation_id, TimePoint start, TimePoint end) : TimeInterval(start, end), operation_id_(operation_id) {}
    size_t operation_id() const {
        return operation_id_;
    }
private:
    size_t operation_id_;
};
