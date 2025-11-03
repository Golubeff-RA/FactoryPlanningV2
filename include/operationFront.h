#pragma once
#include "operation.h"
#include "problem_data.h"
#include "defines.h"
#include <vector>
#include <concepts>

template <typename T>
concept CanSort = requires(T obj, const ProblemData& data, IdsVec& front, const IdsSet& tools) {
    { obj.SortFront(data, front, tools) } -> std::same_as<void>;
};

template <CanSort Sorter>
class OperationFront {
public:
    OperationFront(std::vector<Operation>& operations);
    const IdsVec& front();
    void erase(size_t id);
    void sort(const ProblemData& data, const IdsSet& tools);

private:
    void fillFront();
    IdsVec front_;
    std::vector<Operation> operations_;
    Sorter sorter_;
};

template <CanSort Sorter>
OperationFront<Sorter>::OperationFront(std::vector<Operation>& operations) 
    : operations_(operations) {
    fillFront();
}

template <CanSort Sorter>
const IdsVec& OperationFront<Sorter>::front() {
    return front_;
}

template <CanSort Sorter>
void OperationFront<Sorter>::erase(size_t id) {
    //O(n)
    auto findId = std::find(front_.begin(), front_.end(), id);
    if (findId == front_.end()) {
        //throw std::runtime_error("front not have that id" + std::to_string(id));
        return;
    }
    
    front_.erase(findId);
    for (auto ind : operations_[id].dependencies()) {
        auto op = operations_[ind];
        op.DelDependency(id, START_TIME_POINT);
        if (!op.HasDependencies()) {
            front_.push_back(op.id());
        }
    }
}

template <CanSort Sorter>
void OperationFront<Sorter>::sort(const ProblemData& data, const IdsSet& tools) {
    sorter_.SortFront(data, front_, tools);
}

template <CanSort Sorter>
void OperationFront<Sorter>::fillFront() {
    for(size_t i = 0; i < operations_.size(); ++i) {
        if(!operations_[i].HasDependencies()) {
            front_.push_back(i);
        }
    }
}