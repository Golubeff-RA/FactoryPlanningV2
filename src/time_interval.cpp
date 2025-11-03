#include "time_interval.h"

TimeInterval::TimeInterval(TimePoint start, TimePoint end) : start_(start), end_(end) {}

TimePoint TimeInterval::start() const { return start_; }

TimePoint TimeInterval::end() const { return end_; }

Duration TimeInterval::GetTimeSpan(TimePoint stamp) const {
    if (stamp == START_TIME_POINT) {
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

bool TimeInterval::operator<(const TimeInterval& other) const { return start_ < other.start_; }

bool TimeInterval::operator==(const TimeInterval& other) const {
    return start_ <= other.start_ && other.end_ <= end_;
}

bool TimeInterval::operator!=(const TimeInterval& other) const {
    return !(this->operator==(other));
}

NamedTimeInterval::NamedTimeInterval(size_t operation_id, TimePoint start, TimePoint end)
    : TimeInterval(start, end), operation_id_(operation_id) {}

size_t NamedTimeInterval::operation_id() const { return operation_id_; }