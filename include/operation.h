#pragma once
#include "defines.h"
#include "vector"
#include "work.h"

class Operation {
public:
    Operation(size_t id, bool stoppable) : id_(id), stoppable_(stoppable) {}

    bool AddDependency(size_t id) {
        if (dependencies_.insert(id).second) {
            ++cnt_deps_;
            return true;
        }
        return false;
    }

    bool AddDepended(size_t id) { return depended_.insert(id).second; }

    bool AddPossibleTool(size_t id) {
        return possible_tools_.insert(id).second;
    }

    bool HasDependencies() { return cnt_deps_ == 0; }

    bool Appointed() {
        return start_time_ != START_TIME_POINT && end_time_ != START_TIME_POINT;
    }

    bool SetTimes(TimePoint start, TimePoint end, Duration span,
                  std::vector<Operation>& all_operations) {
        if (start > end || end - start < span) {
            return false;
        }

        start_time_ = start;
        end_time_ = end;

        // мы назначили время выполнения опреации, значит надо уменьшить число
        // зависимостей для всех зависимых
        for (size_t dep_id : depended_) {
            all_operations[dep_id].DelDependency(id_, end);
        }
        return true;
    }

    void SetWorkPtr(Work* work) { ptr_to_work = work; }

    StEndTimes GetStEndTimes() { return {start_time_, end_time_}; }

    bool CanBeAppointed(TimePoint stamp) {
        return !Appointed() && cnt_deps_ == 0 && stamp > possible_start &&
               stamp > ptr_to_work->start_time();
    }

    bool DelDependency(size_t dep_id, TimePoint dep_end) {
        if (dependencies_.erase(dep_id) != 1) {
            return false;
        }
        --cnt_deps_;
        possible_start = std::max(possible_start, dep_end);
        return true;
    }

    bool stoppable() const {
        return stoppable_;
    }

    size_t id() const {
        return id_;
    }

private:
    Work* ptr_to_work = nullptr;
    size_t id_;       // номер операции
    bool stoppable_;  // прерываемость
    size_t cnt_deps_;  // число неназначенных родителей
    IdsSet depended_;      // множество зависимых от неё
    IdsSet dependencies_;  // множество зависимостей
    IdsSet possible_tools_;  // множество возможных исполнителей
    TimePoint start_time_ = START_TIME_POINT;
    TimePoint end_time_ = START_TIME_POINT;
    TimePoint possible_start =
        START_TIME_POINT;  // момент возможного старта операции
};
