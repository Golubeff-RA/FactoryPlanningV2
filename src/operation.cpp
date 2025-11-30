#include "basics/operation.h"

#include "defines.h"

Operation::Operation(size_t id, bool stoppable)
    : id_(id), stoppable_(stoppable) {
    possible_starts_.push(kStartTimePoint);
}

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

bool Operation::HasDependencies() { return cnt_deps_ != 0; }

bool Operation::Appointed() const {
    return start_time_ != kStartTimePoint && end_time_ != kStartTimePoint;
}

bool Operation::SetTimes(TimePoint start, TimePoint end, Duration span,
                         std::vector<Operation>& all_operations) {
    start_time_ = start;
    end_time_ = end;

    // мы назначили время выполнения операции, значит надо уменьшить число
    // зависимостей для всех зависимых
    for (size_t dep_id : depended_) {
        all_operations[dep_id].DelDependency(id_, end);
    }
    return true;
}

void Operation::SetWorkPtr(WorkPtr work) {
    ptr_to_work_ = work;
    work->AddOperation(ID());
}

StEndTimes Operation::GetStEndTimes() const { return {start_time_, end_time_}; }

bool Operation::CanBeAppointed(TimePoint stamp) const {
    return !Appointed() && dependencies_.empty() &&
           stamp >= ptr_to_work_->StartTime() &&
           stamp >= possible_starts_.top();
}

bool Operation::DelDependency(size_t dep_id, TimePoint dep_end) {
    if (dependencies_.erase(dep_id) != 1) {
        return false;
    }
    --cnt_deps_;
    possible_starts_.push(std::max(possible_starts_.top(), dep_end));
    return true;
}

bool Operation::Stoppable() const { return stoppable_; }

const IdsSet& Operation::PossibleTools() const { return possible_tools_; }
const IdsSet& Operation::Dependencies() const { return dependencies_; }
const IdsSet& Operation::Depended() const { return depended_; }

size_t Operation::ID() const { return id_; }
size_t Operation::CntDeps() const { return cnt_deps_; }

WorkPtr Operation::PtrToWork() const { return ptr_to_work_; }