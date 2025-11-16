#include "solver.h"

#include "defines.h"

TimePointsQueue Solver::GetStartTimes(const ProblemData& data) {
    TimePointsQueue times;
    std::set<TimePoint> times_set;
    for (const auto& work : data.works) {
        times_set.insert(work->start_time());
    }

    for (const auto& tool : data.tools) {
        for (const auto& interval : tool.GetShedule()) {
            times_set.insert(interval.start());
        }
    }

    for (TimePoint point : times_set) {
        times.push(point);
    }

    return times;
}

IdsVec Solver::FindFront(const IdsSet& pre_front, const ProblemData& data,
                         TimePoint current_time) {
    IdsVec front;
    for (size_t oper : pre_front) {
        const Operation& operation = data.operations[oper];
        if (operation.CanBeAppointed(current_time)) {
            front.push_back(operation.id());
        }
    }
    return front;
}

IdsSet Solver::FindPossibleTools(const IdsVec& front, const ProblemData& data,
                                 TimePoint current_time) {
    IdsSet possible_tools;
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
    return possible_tools;
}

void Solver::Appointment(const IdsVec& front, IdsSet& pre_front,
                         IdsSet& possible_tools, TimePoint current_time,
                         ProblemData& data, TimePointsQueue& timestamps) {
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
            if (operation.possible_tools().contains(r) && result.first) {
                if (result.second < best_duration) {
                    best_duration = result.second;
                    best_tool_id = r;
                }
            }
        }
        if (best_tool_id != INT64_MAX) {
            data.tools[best_tool_id].Appoint(
                operation, current_time, data.times_matrix[oper][best_tool_id],
                data.operations);
            timestamps.push(operation.GetStEndTimes().second);
            possible_tools.erase(best_tool_id);
            pre_front.erase(oper);
            pre_front.insert(operation.depended().begin(),
                             operation.depended().end());
        }
    }
}