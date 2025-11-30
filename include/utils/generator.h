#pragma once
#include <vector>

#include "basics/problem_data.h"

constexpr int kDefaultSeed = 52;

class Generator {
public:
    Generator() : rng_(kDefaultSeed) {}
    ProblemData Generate(GenerationParams params);

protected:
    Duration GetRandomDuration(int min, int max) {
        return Duration{ch::seconds{rng_.GetInt(min, max)}};
    }

    static bool CanEdgeBeCreated(
        size_t master_id, size_t slave_id,
        const std::vector<std::pair<size_t, TimeInterval>>& effective_intervals,
        const ProblemData& data);

    void CreateShedules(GenerationParams params, ProblemData& data);

    void CreateOperations(
        GenerationParams params, ProblemData& data,
        std::vector<std::pair<size_t, TimeInterval>>& effective_intervals);

    void CreateEdges(GenerationParams params, ProblemData& data,
                     const std::vector<std::pair<size_t, TimeInterval>>&
                         effective_intervals);
    RandomGenerator rng_;
};