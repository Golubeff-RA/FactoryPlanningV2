#include "tool.h"

Tool::Tool(const std::set<TimeInterval> shedule) : shedule_(shedule) {}
Tool::Tool(std::initializer_list<TimeInterval>&& shedule) : shedule_(shedule) {}

bool Tool::CanStartWork(const Operation& operation, TimePoint stamp, Duration span) {
    auto it = GetStartIterator(stamp);
    if (it == shedule_.end()) {
        return false;
    }

    if (work_process_.back().Intersects({stamp, stamp})) {
        return false;
    }

    if (operation.stoppable()) {
        Duration time{0};
        while (it != shedule_.end() && time < span) {
            time += it->GetTimeSpan(stamp);
            ++it;
            stamp = it->start();
        }
        return time >= span;
    }
    return it->GetTimeSpan(stamp) >= span;
}

void Tool::Appoint(Operation& operation, TimePoint stamp, Duration span,
             std::vector<Operation>& all_operations) {
    Duration time(0);
    auto it = GetStartIterator(stamp);

    TimePoint start_time = stamp;
    while (it != shedule_.end() && time < span) {
        work_process_.push_back(
            {operation.id(), stamp, std::min(it->end(), stamp + span - time)});
        time += it->GetTimeSpan(stamp);
        ++it;
        stamp = it->start();
    }
    TimePoint end_time = work_process_.back().end();
    operation.SetTimes(start_time, end_time, time, all_operations);
}

const std::set<TimeInterval>& Tool::GetShedule() const { return shedule_; }

const std::list<NamedTimeInterval>& Tool::GetWorkProcess() const { return work_process_; }

std::set<TimeInterval>::const_iterator Tool::GetStartIterator(TimePoint stamp) {
    if (shedule_.empty()) {
        return shedule_.end();
    }

    TimeInterval interval(stamp, stamp);
    auto it = shedule_.lower_bound(interval);

    if (it == shedule_.begin() && it->start() > stamp) {
        return shedule_.end();
    }

    if (it == shedule_.end() || it->start() > stamp) {
        it = std::prev(it);
    }

    if (stamp >= it->start() && stamp < it->end()) {
        return it;
    }

    return shedule_.end();
}