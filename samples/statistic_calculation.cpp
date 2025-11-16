#include <fstream>
#include <optional>
#include <thread>

#include "concurent_grid.h"
#include "dataset.h"
#include "printer.h"
#include "problem_data.h"
#include "scorer.h"
#include "solution_checker.h"
#include "solver.h"
#include "sorters.h"

constexpr int kSolverSeed = 1232412;
constexpr double kScoreMult = 100;

template <CanSort Sorter>
double Pipeline(const ProblemData& data) {
    Solver solver;
    ProblemData data_copy = data;
    solver.Solve<Sorter>(data_copy, kSolverSeed);
    SolutionChecker::Check(data_copy);
    Score score = BasicScorer::CalcScore(data_copy, kScoreMult);
    return score.appointed_fine;
}

int main() {
    std::vector<std::string> row_names;
    std::vector<std::string> col_names{"Dummy", "Directive",  "Stoppable",
                                       "Robin", "FineSorter", "Dependent"};
    Dataset dataset("../../dataset_directory", row_names);
    ConcurentGrid<double> table(row_names.size(), col_names.size(), row_names,
                                col_names, 999999);
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&]() {
            Solver solver;
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