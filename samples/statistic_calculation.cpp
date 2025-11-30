#include <fstream>
#include <optional>
#include <thread>

#include "basics/problem_data.h"
#include "gangsta_multistep.h"
#include "multistep_solver.h"
#include "scorers/scorer.h"
#include "solvers/solution_checker.h"
#include "solvers/solver.h"
#include "solvers/sorters.h"
#include "utils/concurent_grid.h"
#include "utils/dataset.h"
#include "utils/printer.h"

constexpr int kSolverSeed = 1232412;
constexpr double kScoreMult = 100;

template <CanSort Sorter>
double Pipeline(const ProblemData& data) {
    ProblemData data_copy = data;
    Solver::Solve<Sorter>(data_copy, kSolverSeed);
    SolutionChecker::Check(data_copy);
    Score score = BasicScorer::CalcScore(data_copy, kScoreMult);
    return score.appointed_fine;
}

template <CanSort Sorter, size_t depth, size_t beam_size>
double PipelineForMultistep(const ProblemData& data) {
    ProblemData data_copy = data;
    data_copy = GangstaMultistep::Solve<Sorter, SimpleScorer>(
        data_copy, depth, beam_size, 123432524);
    SolutionChecker::Check(data_copy);
    Score score = BasicScorer::CalcScore(data_copy, kScoreMult);
    return score.appointed_fine;
}

int main() {
    std::vector<std::string> row_names;
    std::vector<std::string> col_names{
        "Dummy",          "Directive",     "Stoppable",      "Robin",
        "FineSorter",     "Dependent",     "MDummy6_2",      "MDirective6_2",
        "MStoppable6_2",  "MRobin6_2",     "MFineSorter6_2", "MDependent6_2",
        "MDummy2_8",      "MDirective2_8", "MStoppable2_8",  "MRobin2_8",
        "MFineSorter2_8", "MDependent2_8",
    };
    Dataset dataset("../../dataset_directory", row_names);
    ConcurentGrid<double> table(row_names.size(), col_names.size(), row_names,
                                col_names, 999999);
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&]() {
            ProblemData data;
            std::optional<std::string> name{"dummy"};
            while (name.has_value()) {
                name = dataset.GetNext(&data);
                if (name.has_value()) {
                    table.Set(name.value(), "Dummy",
                              Pipeline<DummySorter>(data));
                    table.Set(name.value(), "Directive",
                              Pipeline<DirectiveTimeSorter>(data));
                    table.Set(name.value(), "Stoppable",
                              Pipeline<StoppableSorter>(data));
                    table.Set(name.value(), "Robin",
                              Pipeline<RoundRobinSorter>(data));
                    table.Set(name.value(), "FineSorter",
                              Pipeline<FineSorter>(data));
                    table.Set(name.value(), "Dependent",
                              Pipeline<DependedSorter>(data));
                    table.Set(name.value(), "MDummy6_2",
                              PipelineForMultistep<DummySorter, 6, 2>(data));
                    table.Set(
                        name.value(), "MDirective6_2",
                        PipelineForMultistep<DirectiveTimeSorter, 6, 2>(data));
                    table.Set(
                        name.value(), "MStoppable6_2",
                        PipelineForMultistep<StoppableSorter, 6, 2>(data));
                    table.Set(
                        name.value(), "MRobin6_2",
                        PipelineForMultistep<RoundRobinSorter, 6, 2>(data));
                    table.Set(name.value(), "MFineSorter6_2",
                              PipelineForMultistep<FineSorter, 6, 2>(data));
                    table.Set(name.value(), "MDependent6_2",
                              PipelineForMultistep<DependedSorter, 6, 2>(data));

                    table.Set(name.value(), "MDummy2_8",
                              PipelineForMultistep<DummySorter, 2, 8>(data));
                    table.Set(
                        name.value(), "MDirective2_8",
                        PipelineForMultistep<DirectiveTimeSorter, 2, 8>(data));
                    table.Set(
                        name.value(), "MStoppable2_8",
                        PipelineForMultistep<StoppableSorter, 2, 8>(data));
                    table.Set(
                        name.value(), "MRobin2_8",
                        PipelineForMultistep<RoundRobinSorter, 2, 8>(data));
                    table.Set(name.value(), "MFineSorter2_8",
                              PipelineForMultistep<FineSorter, 2, 8>(data));
                    table.Set(name.value(), "MDependent2_8",
                              PipelineForMultistep<DependedSorter, 2, 8>(data));
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::ofstream out_file("dataset_table.csv");
    Printer::PrintGridCSV(table, out_file);
    return 0;
}