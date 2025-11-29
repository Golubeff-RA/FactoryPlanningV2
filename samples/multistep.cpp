#include <iostream>

#include "basics/problem_data.h"
#include "scorers/scorer.h"
#include "solvers/multistep_solver.h"
#include "solvers/solution_checker.h"
#include "solvers/sorters.h"
#include "utils/generator.h"

constexpr size_t kCntTools = 40;
constexpr size_t kCntIntervals = 100;
constexpr std::pair<int, int> kSpacerDur = {100, 200};
constexpr std::pair<int, int> kIntervalDur = {1000, 2000};
constexpr double kOpGenProb = 0.7;
constexpr double kAddEdgeProb = 0.7;
constexpr double kAddToolProb = 0.5;
constexpr int kSeedGenerator = 65783928;

int main() {
    Generator gen;
    GenerationParams params{
        kCntTools,    kCntIntervals, ch::system_clock::now(),
        kSpacerDur,   kIntervalDur,  kOpGenProb,
        kAddEdgeProb, kAddToolProb,  kSeedGenerator};
    ProblemData data(gen.Generate(params));
    std::cout << "data created!\n";
    ProblemData data2 =
        MultistepSolver::Solve<DummySorter, SimpleScorer>(data, 3, 2);
    SolutionChecker::Check(data2);
    Score score = BasicScorer::CalcScore(data2, 100);
    std::cout << "Multistep: " << score.appointed_fine << '\n';

    ProblemData data_copy = data;

    Solver::Solve<DummySorter>(data_copy, 123432524);
    SolutionChecker::Check(data_copy);

    Score score_2 = BasicScorer::CalcScore(data_copy, 100);
    std::cout << "One step: " << score_2.appointed_fine << '\n';

    return 0;
}