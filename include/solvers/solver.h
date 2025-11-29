#pragma once
#include <algorithm>
#include <functional>
#include <queue>

#include "basics/operation.h"
#include "basics/problem_data.h"
#include "defines.h"
#include "solvers/sorters.h"

using TimePointsQueue = std::priority_queue<TimePoint, std::vector<TimePoint>,
                                            std::greater_equal<TimePoint>>;

class Solver {
public:
    template <CanSort Sorter>
    static void Solve(ProblemData& data, int seed = 185643241) {
        RandomGenerator gena(seed);
        Sorter sorter;
        TimePoint current_time;
        TimePointsQueue timestamps(GetStartTimes(data));

        IdsSet pre_front(CalcPreFront(data));

        while (!timestamps.empty()) {
            current_time = timestamps.top();
            timestamps.pop();

            IdsVec front = FindFront(pre_front, data, current_time);
            IdsSet possible_tools =
                FindPossibleTools(front, data, current_time);
            std::shuffle(front.begin(), front.end(), gena.GetGen());
            sorter.SortFront(data, front, possible_tools);

            // назначение
            // вот здесь надо не первый попавшийся инструмент использовать
            // а для каждой операции в текущем фронте искать самый оптимальный
            Appointment(front, pre_front, possible_tools, current_time, data,
                        timestamps);
        }
    }

protected:
    static IdsSet CalcPreFront(const ProblemData& data) {
        IdsSet pre_front;
        std::for_each(data.operations.begin(), data.operations.end(),
                      [&](const Operation& op) {
                          if (op.dependencies().empty()) {
                              pre_front.insert(op.id());
                          }
                      });
        return pre_front;
    }
    static TimePointsQueue GetStartTimes(const ProblemData& data);

    static IdsVec FindFront(const IdsSet& pre_front, const ProblemData& data,
                            TimePoint current_time);
    static IdsSet FindPossibleTools(const IdsVec& front,
                                    const ProblemData& data,
                                    TimePoint current_time);
    static void Appointment(const IdsVec& front, IdsSet& pre_front,
                            IdsSet& possible_tools, TimePoint current_time,
                            ProblemData& data, TimePointsQueue& timestamps);
    RandomGenerator gena_;
};