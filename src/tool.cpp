#include "tool.h"
#include <iostream>
Tool::Tool(const std::set<TimeInterval> shedule) : shedule_(shedule) {}
Tool::Tool(std::initializer_list<TimeInterval>&& shedule) : shedule_(shedule) {}

bool Tool::CanStartWork(const Operation& operation, TimePoint stamp, Duration span) {
    auto it = GetStartIterator(stamp);
    if (it == shedule_.end()) {
        return false;
    }

    // вот это была полная шляпа, пересекались интервалы
    /*if (work_process_.back().Intersects({stamp, stamp})) {
        return false;
    }*/

    if (IntersectsWithWorkProc(stamp)) {
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
    while (time < span) {
        NamedTimeInterval interval{operation.id(), std::max(stamp, it->start()),
                                   std::min(it->end(), stamp + span - time)};
        if (work_process_.contains(interval)) {
            std::cout << "Warning! False appointment" << std::endl;
        }
        work_process_.insert(interval);
        time += interval.GetTimeSpan();
        ++it;
        stamp = it->start();
    }
    TimePoint end_time = std::prev(work_process_.end())->end();
    operation.SetTimes(start_time, end_time, time, all_operations);
}

const std::set<TimeInterval>& Tool::GetShedule() const { return shedule_; }

const std::set<NamedTimeInterval>& Tool::GetWorkProcess() const { return work_process_; }

bool Tool::IntersectsWithWorkProc(TimePoint stamp) const {
    NamedTimeInterval interval(666, stamp, stamp);
    if (!work_process_.empty()) {
        const auto& last = *work_process_.rbegin();
        if (interval.start() < last.end()) {
            return true;
        }
    }

    for (const auto& i : work_process_) {
        if (i.Intersects(interval)) {
            return true;
        }
    }

    return false;
}
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