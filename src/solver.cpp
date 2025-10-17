#include "solver.h"

std::set<TimePoint> Solver::GetStartTimes(const ProblemData& data) {
    std::set<TimePoint> times;
    for (const auto& work : data.works) {
        times.insert(work->start_time());
    }

    for (const auto& tool : data.tools) {
        for (const auto& interval : tool.GetShedule()) {
            times.insert(interval.start());
        }
    }

    return times;
}