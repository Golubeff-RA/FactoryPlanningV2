#include "solvers/solution_checker.h"

#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include "utils/printer.h"

void SolutionChecker::CollisionDetector(const ProblemData& data) {
    for (size_t i = 0; i < data.tools.size(); ++i) {
        auto tool = data.tools[i];
        for (auto it = tool.GetWorkProcess().begin();
             it != tool.GetWorkProcess().end(); ++it) {
            if (it != tool.GetWorkProcess().begin() &&
                it->Intersects(*std::prev(it))) {
                throw std::runtime_error("Collision");
            }
        }
    }
}

void SolutionChecker::TimeChecker(const ProblemData& data) {
    std::unordered_map<size_t, std::pair<size_t, Duration>> appointed_times;
    for (size_t i = 0; i < data.tools.size(); ++i) {
        const auto& tool = data.tools[i];
        for (const auto& in : tool.GetWorkProcess()) {
            if (!appointed_times.contains(in.OperationID()) &&
                data.operations[in.OperationID()].Appointed()) {
                appointed_times[in.OperationID()] = {i, Duration(0)};
            }

            appointed_times[in.OperationID()].second += in.GetTimeSpan();
        }
    }

    for (auto it = appointed_times.begin(); it != appointed_times.end(); ++it) {
        if (data.times_matrix[it->first][it->second.first] >
            it->second.second) {
            std::stringstream ss;
            ss << "False appointment at operation " << it->first << " tool "
               << it->second.first
               << " real_time = " << DurationToStr(it->second.second)
               << it->second.first << " table_time = "
               << DurationToStr(data.times_matrix[it->first][it->second.first])
               << std::endl;
            throw std::runtime_error(ss.str());
        }
    }
}

void SolutionChecker::HappensBeforeChecker(const ProblemData& data) {
    for (const auto& operation : data.operations) {
        if (!operation.Appointed()) {
            continue;
        }

        for (size_t child_id : operation.Depended()) {
            if (!data.operations[child_id].Appointed()) {
                continue;
            }

            if (operation.GetStEndTimes().second >
                data.operations[child_id].GetStEndTimes().first) {
                std::stringstream ss;
                ss << "operation " << child_id
                   << " appointed earlier than parent #" << operation.ID();
                throw std::runtime_error(ss.str());
            }
        }
    }
}

void SolutionChecker::EarlyStartChecker(const ProblemData& data) {
    for (const auto& operation : data.operations) {
        if (!operation.Appointed()) {
            continue;
        }
        if (operation.GetStEndTimes().first <
            operation.PtrToWork()->StartTime()) {
            throw std::runtime_error(
                "Opeartion starts earlier than her Work can");
        }
    }
}