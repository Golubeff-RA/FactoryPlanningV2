#pragma once
#include "operation.h"
#include "time_interval.h"

class Tool {
public:
    Tool(const std::set<TimeInterval> shedule);
    Tool(std::initializer_list<TimeInterval>&& shedule);
    bool CanStartWork(const Operation& operation, TimePoint stamp,
                      Duration span);
    void Appoint(Operation& operation, TimePoint stamp, Duration span,
                 std::vector<Operation>& all_operations);
    const std::set<TimeInterval>& GetShedule() const;
    const std::set<NamedTimeInterval>& GetWorkProcess() const;

private:
    std::set<TimeInterval> shedule_;
    std::set<NamedTimeInterval> work_process_;
    std::set<TimeInterval>::const_iterator GetStartIterator(TimePoint stamp);
    bool IntersectsWithWorkProc(TimePoint timestamp) const;
};
