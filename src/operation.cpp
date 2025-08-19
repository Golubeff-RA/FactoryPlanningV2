#include "operation.h"

Operation::Operation(size_t id, bool stoppable)
    : id_(id), stoppable_(stoppable) {}

bool Operation::AddDependency(size_t id) {
    if (dependencies_.insert(id).second) {
        ++cnt_deps_;
        return true;
    }
    return false;
}

bool Operation::AddDepended(size_t id) { return depended_.insert(id).second; }

bool Operation::AddPossibleTool(size_t id) {
    return possible_tools_.insert(id).second;
}

bool Operation::HasDependencies() { return cnt_deps_ == 0; }

bool Operation::Appointed() const {
    return start_time_ != START_TIME_POINT || end_time_ != START_TIME_POINT;
}

bool Operation::SetTimes(TimePoint start, TimePoint end, Duration span,
                         std::vector<Operation>& all_operations) {
    start_time_ = start;
    end_time_ = end;

    // мы назначили время выполнения опреации, значит надо уменьшить число
    // зависимостей для всех зависимых
    for (size_t dep_id : depended_) {
        all_operations[dep_id].DelDependency(id_, end);
    }
    return true;
}

void Operation::SetWorkPtr(Work* work) { ptr_to_work_ = work; }

StEndTimes Operation::GetStEndTimes() const { return {start_time_, end_time_}; }

bool Operation::CanBeAppointed(TimePoint stamp) const {
    return !Appointed() && cnt_deps_ == 0 && stamp > possible_start &&
           stamp > ptr_to_work_->start_time();
}

bool Operation::DelDependency(size_t dep_id, TimePoint dep_end) {
    if (dependencies_.erase(dep_id) != 1) {
        return false;
    }
    --cnt_deps_;
    possible_start = std::max(possible_start, dep_end);
    return true;
}

bool Operation::stoppable() const { return stoppable_; }

const IdsSet& Operation::possible_tools() const { return possible_tools_; }

size_t Operation::id() const { return id_; }

size_t Operation::cnt_deps() const { return cnt_deps_; }

const Work* Operation::ptr_to_work() const { return ptr_to_work_; }