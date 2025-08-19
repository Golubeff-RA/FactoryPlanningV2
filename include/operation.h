#pragma once
#include <vector>

#include "defines.h"
#include "work.h"

class Operation {
public:
    Operation(size_t id, bool stoppable);

    bool AddDependency(size_t id);

    bool AddDepended(size_t id);

    bool AddPossibleTool(size_t id);

    bool HasDependencies();

    bool Appointed() const;

    bool SetTimes(TimePoint start, TimePoint end, Duration span,
                  std::vector<Operation>& all_operations);

    void SetWorkPtr(Work* work);

    StEndTimes GetStEndTimes() const;

    bool CanBeAppointed(TimePoint stamp) const;

    bool DelDependency(size_t dep_id, TimePoint dep_end);
    bool stoppable() const;

    const IdsSet& possible_tools() const;
    size_t id() const;
    size_t cnt_deps() const;

    const Work* ptr_to_work() const;

private:
    Work* ptr_to_work_ = nullptr;
    size_t id_;       // номер операции
    bool stoppable_;  // прерываемость
    size_t cnt_deps_ = 0;  // число неназначенных родителей
    IdsSet depended_;      // множество зависимых от неё
    IdsSet dependencies_;  // множество зависимостей
    IdsSet possible_tools_;  // множество возможных исполнителей
    TimePoint start_time_ = START_TIME_POINT;
    TimePoint end_time_ = START_TIME_POINT;
    TimePoint possible_start =
        START_TIME_POINT;  // момент возможного старта операции
};
