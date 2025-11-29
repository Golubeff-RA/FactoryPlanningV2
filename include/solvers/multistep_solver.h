#pragma once
#include <limits.h>

#include <algorithm>
#include <cfloat>
#include <cstddef>
#include <deque>
#include <iostream>

#include "basics/operation.h"
#include "basics/problem_data.h"
#include "defines.h"
#include "solvers/solver.h"
#include "solvers/sorters.h"

template <class T>
concept CanScoreInter = requires(const ProblemData& data) {
    { T::CalcScore(data) } -> std::same_as<Score>;
};

struct SimpleScorer {
    static Score CalcScore(const ProblemData& data) {
        Score answer{0, 0};
        for (auto work : data.works) {
            TimePoint last_op_end = START_TIME_POINT;
            size_t not_appointed_ops = 0;
            for (size_t idx : work->operation_ids()) {
                last_op_end = std::max(
                    data.operations[idx].GetStEndTimes().second, last_op_end);
                if (!data.operations[idx].Appointed()) {
                    ++not_appointed_ops;
                }
            }
            answer.appointed_fine +=
                work->fine_coef() *
                std::chrono::duration<double>((last_op_end - work->directive()))
                    .count();
            answer.not_appointed_fine += work->fine_coef() * not_appointed_ops;
        }
        // std::cout << answer.appointed_fine << " " <<
        // answer.not_appointed_fine
        //           << std::endl;
        return answer;
    }
};

struct DataAndTimes {
    ProblemData data;
    TimePointsQueue timers;
    IdsSet pre_front;
};

using ProblemDatasTree = std::deque<DataAndTimes>;

class MultistepSolver : Solver {
public:
    template <CanSort OneStepSorter, CanScoreInter Scorer>
    static ProblemData Solve(const ProblemData& data, size_t step_count,
                             size_t vertex_degree, int seed = 123432524) {
        RandomGenerator gena(seed);
        OneStepSorter sorter;
        ProblemData data_answer;
        ProblemDatasTree problem_datas;
        for (size_t i = 0; i < vertex_degree; ++i) {
            problem_datas.push_back(
                {data, GetStartTimes(data), CalcPreFront(data)});
        }
        DataAndTimes best_data;
        do {
            // построим дерево степени vertex_degree и глубиной step_count
            for (size_t step = 1; step <= step_count; ++step) {
                for (size_t count = 0; count < std::pow(vertex_degree, step);
                     ++count) {
                    if (problem_datas.front().timers.empty()) {
                        continue;
                    }
                    TimePoint current_time = problem_datas.front().timers.top();
                    problem_datas.front().timers.pop();
                    DataAndTimes& pack = problem_datas.front();
                    IdsVec front =
                        FindFront(pack.pre_front, pack.data, current_time);
                    IdsSet possible_tools =
                        FindPossibleTools(front, pack.data, current_time);

                    std::shuffle(front.begin(), front.end(), gena.GetGen());

                    sorter.SortFront(pack.data, front, possible_tools);
                    Appointment(front, pack.pre_front, possible_tools,
                                current_time, pack.data, pack.timers);

                    for (size_t i = 0; i < vertex_degree; ++i) {
                        problem_datas.push_back(pack);
                    }
                    problem_datas.pop_front();
                }
            }

            Score best_score{DBL_MAX, DBL_MAX};
            size_t best_problem_id = 0;
            // возьмём лучшую задачу из дерева
            for (size_t i = 0; i < problem_datas.size(); ++i) {
                Score score = Scorer::CalcScore(problem_datas[i].data);
                if ((score < best_score)) {
                    best_score = score;
                    best_problem_id = i;
                }
            }

            best_data = {problem_datas[best_problem_id]};
            problem_datas.clear();

            for (size_t i = 0; i < vertex_degree; ++i) {
                problem_datas.push_back(best_data);
            }
            std::cout << best_data.pre_front.size() << "\n";
        } while (!AllTimersEmpty(problem_datas) &&
                 !best_data.pre_front.empty());
        return problem_datas.front().data;
    }

private:
    static bool AllTimersEmpty(const ProblemDatasTree& tree) {
        return std::all_of(
            tree.begin(), tree.end(),
            [&](const DataAndTimes& data) { return data.timers.empty(); });
    }

    static bool AllOpperationsAppointed(const ProblemData& data) {
        return std::all_of(data.operations.begin(), data.operations.end(),
                           [&](const Operation& op) { return op.Appointed(); });
    }
};