#pragma once
#include <concepts>
#include <deque>
#include <unordered_map>

#include "basics/problem_data.h"

template <typename T>
concept CanSort = requires(T obj, const ProblemData& data, IdsVec& front,
                           const IdsSet& tools) {
    { obj.SortFront(data, front, tools) } -> std::same_as<void>;
};

struct DummySorter {
    void SortFront(const ProblemData& data, IdsVec& front,
                   const IdsSet& tools) {}
};

struct DirectiveTimeSorter {
    void SortFront(const ProblemData& data, IdsVec& front,
                   const IdsSet& tools) {
        std::sort(front.begin(), front.end(), [&](size_t a, size_t b) {
            return data.operations[a].ptr_to_work()->directive() <
                   data.operations[b].ptr_to_work()->directive();
        });
    }
};

struct StoppableSorter {
    void SortFront(const ProblemData& data, IdsVec& front,
                   const IdsSet& tools) {
        std::sort(front.begin(), front.end(), [&](size_t a, size_t b) {
            return data.operations[a].stoppable() >
                   data.operations[b].stoppable();
        });
    }
};

struct FineSorter {
    void SortFront(const ProblemData& data, IdsVec& front,
                   const IdsSet& tools) {
        std::sort(front.begin(), front.end(), [&](size_t a, size_t b) {
            return data.operations[a].ptr_to_work()->fine_coef() >
                   data.operations[b].ptr_to_work()->fine_coef();
        });
    }
};

struct DependedSorter {
    void SortFront(const ProblemData& data, IdsVec& front,
                   const IdsSet& tools) {
        std::sort(front.begin(), front.end(), [&](size_t a, size_t b) {
            return data.operations[a].depended().size() >
                   data.operations[b].depended().size();
        });
    }
};

struct RoundRobinSorter {
    void SortFront(const ProblemData& data, IdsVec& front,
                   const IdsSet& tools) {
        if (works_q.empty()) {
            for (auto work : data.works) {
                works_q.push_back(work->id());
            }
        }
        std::unordered_map<size_t, size_t> position_map;

        for (size_t i = 0; i < works_q.size(); ++i) {
            position_map[works_q[i]] = i;
        }

        std::sort(front.begin(), front.end(), [&](size_t a, size_t b) {
            return position_map[data.operations[a].ptr_to_work()->id()] <
                   position_map[data.operations[b].ptr_to_work()->id()];
        });

        works_q.push_back(works_q.front());
        works_q.pop_front();
    }

    std::deque<size_t> works_q;
};
