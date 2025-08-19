#pragma once

#include "problem_data.h"

class Solver {
public:
    static void Solve(ProblemData& data);

private:
    static void SortFront(const ProblemData& data, IdsSet front);
    static std::set<TimePoint> GetStartTimes(const ProblemData& data);
};