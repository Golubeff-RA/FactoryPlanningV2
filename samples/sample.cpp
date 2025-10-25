#include <time.h>

#include <fstream>
#include <iostream>
#include <random>

#include "generator.h"
#include "printer.h"
#include "problem_data.h"
#include "scorer.h"
#include "solver.h"

using namespace std::chrono_literals;

void CollisionDetector(const ProblemData& data) {
    for (size_t i = 0; i < data.tools.size(); ++i) {
        auto tool = data.tools[i];
        for (auto it = tool.GetWorkProcess().begin(); it != tool.GetWorkProcess().end(); ++it) {
            if (it != tool.GetWorkProcess().begin() && it->Intersects(*std::prev(it))) {
                throw std::runtime_error("Collision");
            }
        }
    }
}

void TimeChecker(const ProblemData& data) {
    std::unordered_map<size_t, std::pair<size_t, Duration>> appointed_times;
    for (size_t i = 0; i < data.tools.size(); ++i) {
        const auto& tool = data.tools[i];
        for (const auto& in : tool.GetWorkProcess()) {
            if (!appointed_times.contains(in.operation_id()) &&
                data.operations[in.operation_id()].Appointed()) {
                appointed_times[in.operation_id()] = {i, Duration(0)};
            }

            appointed_times[in.operation_id()].second += in.GetTimeSpan();
        }
    }

    for (auto it = appointed_times.begin(); it != appointed_times.end(); ++it) {
        if (data.times_matrix[it->first][it->second.first] > it->second.second) {
            std::cout << "False appointment at operation " << it->first << " tool "
                      << it->second.first << " real_time = " << DurationToStr(it->second.second)
                      << it->second.first << " table_time = "
                      << DurationToStr(data.times_matrix[it->first][it->second.first]) << std::endl;
        }
    }
}

int main() {
    Score score;
    Generator gen;
    Solver solver;
    std::ofstream out_dummy("solution_dummy.json");
    std::ofstream out_directive("solution_directive.json");
    std::ofstream out_robin("solution_robin.json");
    std::ofstream out_stoppable("solution_stoppable.json");
    GenerationParams params{
        20, 30, ch::system_clock::now(), {100, 200}, {1000, 2000}, 0.3, 0.5, 0.4, 65783928};
    ProblemData data(gen.Generate(params));
    std::cout << "data created!\n";

    ProblemData data_copy2 = data;
    solver.Solve<DummySorter>(data_copy2, 1232412);
    score = BasicScorer::CalcScore(data_copy2, 100);
    Printer::PrintAnswerJSON(data_copy2, score, out_dummy);
    std::cout << "Dummy:       " << score.appointed_fine << " || " << score.not_appointed_fine
              << std::endl;
    CollisionDetector(data_copy2);
    TimeChecker(data_copy2);

    data_copy2 = data;
    solver.Solve<DirectiveTimeSorter>(data_copy2, 1232412);
    score = BasicScorer::CalcScore(data_copy2, 100);
    Printer::PrintAnswerJSON(data_copy2, score, out_directive);
    std::cout << "Directive:   " << score.appointed_fine << " || " << score.not_appointed_fine
              << std::endl;

    data_copy2 = data;
    solver.Solve<RoundRobinSorter>(data_copy2, 1232412);
    score = BasicScorer::CalcScore(data_copy2, 100);
    Printer::PrintAnswerJSON(data_copy2, score, out_robin);
    std::cout << "Round robin: " << score.appointed_fine << " || " << score.not_appointed_fine
              << std::endl;

    data_copy2 = data;
    solver.Solve<DirectiveTimeSorter>(data_copy2, 1232412);
    score = BasicScorer::CalcScore(data_copy2, 100);
    Printer::PrintAnswerJSON(data_copy2, score, out_stoppable);
    std::cout << "Stoppable:   " << score.appointed_fine << " || " << score.not_appointed_fine
              << std::endl;
    return 0;
}