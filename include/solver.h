#pragma once
#include <algorithm>
#include <concepts>
#include <deque>
#include <unordered_map>

#include "problem_data.h"
#include "generator.h"

#include <iostream>

template <typename T>
concept CanSort = requires(T obj, const ProblemData& data, IdsVec front, const IdsSet& tools) {
    { obj.SortFront(data, front, tools) } -> std::same_as<void>;
};


inline void PrintVector1(IdsVec vec) {
    for (auto id : vec) {
        std::cout << id << ' ';
    }
    if (!vec.empty())
        std::cout << std::endl;
}

struct DummySorter {
    void SortFront(const ProblemData& data, IdsVec front, const IdsSet& tools) {

    }
};

struct DirectiveTimeSorter {
    void SortFront(const ProblemData& data, IdsVec front, const IdsSet& tools) {
        //PrintVector1(front);
        std::sort(front.begin(), front.end(), [&](size_t a, size_t b) {
            return data.operations[a].ptr_to_work()->directive() <
                   data.operations[b].ptr_to_work()->directive();
        });
        //PrintVector1(front);
    }
};

struct RoundRobinSorter {
    void SortFront(const ProblemData& data, IdsVec front, const IdsSet& tools) {
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

class Solver {
public:
    Solver() : gena_(52) {
    }

    template <CanSort Sorter>
    void Solve(ProblemData& data) {
        Sorter sorter;
        TimePoint current_time;
        std::set<TimePoint> timestamps = GetStartTimes(data);

        while (!timestamps.empty()) {
            IdsSet R;
            IdsVec F;
            current_time = *timestamps.begin();
            timestamps.erase(timestamps.begin());

            // вот тут по идее надо по-умному собирать фронт на основе
            // зависимостей хранящихся в операциях, но я хлебушек и не знаю
            for (const auto& operation : data.operations) {
                if (operation.CanBeAppointed(current_time)) {
                    F.push_back(operation.id());
                }
            }

            std::shuffle(F.begin(), F.end(), gena_.GetGen());
            sorter.SortFront(data, F, R);
            //  сборка возможных исполнителей
            for (size_t oper : F) {
                for (size_t i : data.operations[oper].possible_tools()) {
                    if (data.tools[i].CanStartWork(data.operations[oper], current_time,
                                                   data.times_matrix[oper][i])) {
                        R.insert(i);
                    }
                }
            }

            // назначение
            for (size_t oper : F) {
                auto& operation = data.operations[oper];
                for (size_t r : R) {
                    if (operation.possible_tools().contains(r) &&
                        data.tools[r].CanStartWork(operation, current_time,
                                                   data.times_matrix[oper][r])) {
                        data.tools[r].Appoint(operation, current_time, data.times_matrix[oper][r],
                                              data.operations);
                        timestamps.insert(operation.GetStEndTimes().second);
                        R.erase(r);
                        break;
                    }
                }
            } 
        }
    }

private:
    static std::set<TimePoint> GetStartTimes(const ProblemData& data);
    RandomGenerator gena_;
};