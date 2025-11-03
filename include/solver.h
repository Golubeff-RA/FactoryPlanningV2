#pragma once
#include <algorithm>
#include <concepts>
#include <deque>
#include <unordered_map>

#include "problem_data.h"

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

class Solver {
public:
    Solver() : gena_(185643241) {}

    template <CanSort Sorter>
    void Solve(ProblemData& data, int seed = 185643241) {
        gena_ = RandomGenerator(seed);
        TimePoint current_time;
        std::set<TimePoint> timestamps = GetStartTimes(data);
        Sorter sorter;

        while (!timestamps.empty()) {
            IdsSet possible_tools;
            IdsVec front;
            current_time = *timestamps.begin();
            timestamps.erase(timestamps.begin());

            // вот тут по идее надо по-умному собирать фронт на основе
            // зависимостей хранящихся в операциях, но я хлебушек и не знаю
            for (const auto& operation : data.operations) {
                if (operation.CanBeAppointed(current_time)) {
                    front.push_back(operation.id());
                }
            }

            // std::shuffle(F.begin(), F.end(), gena_.GetGen());

            std::shuffle(front.begin(), front.end(), gena_.GetGen());
            sorter.SortFront(data, front, possible_tools);
            //  сборка возможных исполнителей
            for (size_t oper : front) {
                for (size_t i : data.operations[oper].possible_tools()) {
                    if (data.tools[i]
                            .CanStartWork(data.operations[oper], current_time,
                                          data.times_matrix[oper][i])
                            .first) {
                        possible_tools.insert(i);
                    }
                }
            }

            // назначение
            // вот здесь надо не первый попавшийся инструмент использовать
            // а для каждой операции в текущем фронте искать самый оптимальный
            for (size_t oper : front) {
                auto& operation = data.operations[oper];
                if (!operation.CanBeAppointed(current_time)) {
                    continue;
                }

                size_t best_tool_id = INT64_MAX;
                Duration best_duration = Duration(INT64_MAX);
                for (size_t r : possible_tools) {
                    auto result = data.tools[r].CanStartWork(
                        operation, current_time, data.times_matrix[oper][r]);
                    if (operation.possible_tools().contains(r) &&
                        result.first) {
                        if (result.second < best_duration) {
                            best_duration = result.second;
                            best_tool_id = r;
                        }
                    }
                }
                if (best_tool_id != INT64_MAX) {
                    data.tools[best_tool_id].Appoint(
                        operation, current_time,
                        data.times_matrix[oper][best_tool_id], data.operations);
                    timestamps.insert(operation.GetStEndTimes().second);
                    possible_tools.erase(best_tool_id);
                }
            }
        }
    }

private:
    static std::set<TimePoint> GetStartTimes(const ProblemData& data);
    RandomGenerator gena_;
};