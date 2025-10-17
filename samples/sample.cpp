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
    Generator gen;
    BasicScorer scorer;
    Solver solver;
    GenerationParams params{100, 10, ch::system_clock::now(), 500, 1000, 1000, 1500, 0.7, 100};
    ProblemData data(gen.Generate(params));
    std::cout << "data created!\n";

    ProblemData data_copy2 = data;
    solver.Solve<DirectiveTimeSorter>(data_copy2);
    Score score = scorer.CalcScore(data_copy2, 100);
    std::cout << "Score - appointed_fine = " << score.appointed_fine
              << " not_appointed_fine = " << score.not_appointed_fine << std::endl;
    std::ofstream out("solution.json");
    Printer::PrintAnswerJSON(data_copy2, out);
    CollisionDetector(data_copy2);
    TimeChecker(data_copy2);

    return 0;
}