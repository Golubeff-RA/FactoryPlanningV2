#pragma once
#include <cfloat>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <stack>

#include "basics/problem_data.h"
#include "defines.h"
#include "gangsta_multistep.h"
#include "multistep_solver.h"
#include "utils/count_set.h"

using namespace std::chrono_literals;

class MultiSorter : public GangstaMultistep {
public:
    template <typename Sorter, CanScoreInter Scorer>
    static ProblemData Solve(const ProblemData& data, size_t max_depth,
                             int seed = 123432524) {
        std::mt19937 gen(seed);
        Sorter sorter;
        const size_t beam_size = sorter.Size();
        ProblemData answer = data;
        TimePointsQueue timestamps(GetStartTimes(data));
        CountSet<size_t> pre_front(CalcPreFront(data));
        NodePtr start = std::make_shared<Node>(Node{{}, {}});

        while (!AllOpperationsAppointed(answer) && !timestamps.empty()) {
            NodePtr best_leaf;
            Score best_score{DBL_MAX, DBL_MAX};

            std::stack<std::pair<NodePtr, size_t>> traversal_stack;
            traversal_stack.push({start, 0});
            while (!traversal_stack.empty()) {
                auto& [current_node, next_child_id] = traversal_stack.top();
                if (next_child_id == 0) {
                    TimePoint current_time{*timestamps.begin()};
                    timestamps.erase(current_time);

                    IdsVec front = FindFront(pre_front, answer, current_time);
                    IdsSet possible_tools =
                        FindPossibleTools(front, answer, current_time);
                    std::shuffle(front.begin(), front.end(), gen);
                    size_t current_sorter_idx = 0;
                    if (traversal_stack.size() > 1) {
                        auto copy = traversal_stack;
                        copy.pop();
                        current_sorter_idx =
                            copy.top().first->childs.size() - 1;
                    }
                    gen.seed(gen());
                    sorter.SortFront(data, front, possible_tools,
                                     current_sorter_idx);
                    current_node->appointments =
                        AppointmentProcess(front, pre_front, possible_tools,
                                           current_time, answer, timestamps);
                }

                size_t current_depth = traversal_stack.size();

                if (current_depth >= max_depth || next_child_id >= beam_size) {
                    if (current_depth >= max_depth) {
                        Score score{Scorer::CalcScore(answer)};
                        if (score < best_score) {
                            best_score = score;
                            best_leaf = current_node;
                        }
                    }
                    UndoChanges(answer, timestamps, pre_front, current_node);
                    traversal_stack.pop();
                    continue;
                }

                NodePtr child = std::make_shared<Node>(Node{{}, {}});
                current_node->childs.push_back(child);
                ++next_child_id;
                traversal_stack.push({child, 0});
            }

            // вот тут найдем путь до лучшего "листа" (т. е. все назначения)
            // поставим start на лучшую ноду и от неё построим следующее дерево
            std::stack<NodePtr> way_to_best_reverse;
            FindPath(start, best_leaf, way_to_best_reverse);
            std::vector<NodePtr> way_to_best(ReverseStack(way_to_best_reverse));

            for (NodePtr node : way_to_best) {
                auto appointments = ReverseStack(node->appointments);
                for (Appointment app : appointments) {
                    answer.tools[app.tool_idx].Appoint(
                        answer.operations[app.operation_idx], app.start,
                        answer.times_matrix[app.operation_idx][app.tool_idx],
                        answer.operations);
                    timestamps.insert(app.end);
                    timestamps.erase(app.start);
                    for (size_t id :
                         data.operations[app.operation_idx].Depended()) {
                        pre_front.Insert(id, 1);
                    }
                }
            }

            start = best_leaf;
            start->appointments = {};
        }
        return answer;
    }
};