#pragma once
#include "problem_data.h"

class SolutionChecker {
public:
    static void Check(const ProblemData& data) {
        CollisionDetector(data);
        TimeChecker(data);
        HappensBeforeChecker(data);
        EarlyStartChecker(data);
    }

private:
    static void CollisionDetector(const ProblemData& data);
    static void TimeChecker(const ProblemData& data);
    static void HappensBeforeChecker(const ProblemData& data);
    static void EarlyStartChecker(const ProblemData& data);
};