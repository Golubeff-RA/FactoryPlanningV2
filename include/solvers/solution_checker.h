#pragma once
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "basics/problem_data.h"
#include "defines.h"

class SolutionChecker {
public:
    static void Check(const ProblemData& data) {
        CollisionDetector(data);
        TimeChecker(data);
        HappensBeforeChecker(data);
        EarlyStartChecker(data);
        StoppableChecker(data);
        ToolOperationChecker(data);
    }

private:
    static void CollisionDetector(const ProblemData& data);
    static void TimeChecker(const ProblemData& data);
    static void HappensBeforeChecker(const ProblemData& data);
    static void EarlyStartChecker(const ProblemData& data);
    static void StoppableChecker(const ProblemData& data) {
        std::unordered_map<size_t, size_t> intervals_counters;
        for (const auto& tool : data.tools) {
            for (const auto& interval : tool.GetWorkProcess()) {
                if (!intervals_counters.contains(interval.OperationID())) {
                    intervals_counters.insert({interval.OperationID(), 0});
                }
                intervals_counters[interval.OperationID()] += 1;
            }
        }

        for (auto pair : intervals_counters) {
            if (!data.operations[pair.first].Stoppable() && pair.second != 1) {
                throw std::runtime_error(
                    "Stoppabe operation " + std::to_string(pair.first) +
                    " doned by" + std::to_string(pair.second) + " intervals");
            }
        }
    }
    static void ToolOperationChecker(const ProblemData& data) {
        for (size_t i = 0; i < data.tools.size(); ++i) {
            const Tool& tool = data.tools[i];
            for (const auto& interval : tool.GetWorkProcess()) {
                if (data.times_matrix[interval.OperationID()][i] ==
                    Duration(0)) {
                    throw std::runtime_error(
                        "Y cant do operation " +
                        std::to_string(interval.OperationID()) + " by" +
                        std::to_string(i) + " tool");
                }
            }
        }
    }
};