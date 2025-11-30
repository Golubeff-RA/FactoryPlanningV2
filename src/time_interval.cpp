#include "basics/time_interval.h"

#include "defines.h"


TimeInterval::TimeInterval(TimePoint start, TimePoint end)
    : start_(start), end_(end) {}

TimePoint TimeInterval::Start() const { return start_; }

TimePoint TimeInterval::End() const { return end_; }

Duration TimeInterval::GetTimeSpan(TimePoint stamp) const {
    if (stamp == kStartTimePoint) {
        return end_ - start_;
    }

    if (end_ > stamp) {
        return end_ - stamp;
    }

    return Duration(0);
}

bool TimeInterval::Intersects(const TimeInterval& other) const {
    return !(end_ <= other.start_ || other.end_ <= start_);
}

bool TimeInterval::operator<(const TimeInterval& other) const {
    return start_ < other.start_;
}

bool TimeInterval::operator==(const TimeInterval& other) const {
    return start_ <= other.start_ && other.end_ <= end_;
}

bool TimeInterval::operator!=(const TimeInterval& other) const {
    return !(this->operator==(other));
}

NamedTimeInterval::NamedTimeInterval(size_t operation_id, TimePoint start,
                                     TimePoint end)
    : TimeInterval(start, end), operation_id_(operation_id) {}

size_t NamedTimeInterval::OperationID() const { return operation_id_; }