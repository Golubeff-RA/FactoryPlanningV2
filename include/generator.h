#pragma once
#include <vector>

#include "problem_data.h"

constexpr int kDefaultSeed = 52;

class Generator {
public:
    Generator() : rng_(kDefaultSeed) {}
    ProblemData Generate(GenerationParams params);

private:
    Duration GetRandomDuration(int min, int max) {
        return Duration{ch::seconds{rng_.GetInt(min, max)}};
    }

    // проверяет возможность установки связи между операциями master_id ->
    // slave_id
    static bool CanEdgeBeCreated(
        size_t master_id, size_t slave_id,
        const std::vector<std::pair<size_t, TimeInterval>>& effective_intervals,
        const ProblemData& data);

    void CreateShedules(GenerationParams params, ProblemData& data);

    void CreateOperations(GenerationParams params, ProblemData& data,
                          std::vector<std::pair<size_t, TimeInterval>>& effective_intervals);

    void CreateEdges(GenerationParams params, ProblemData& data,
                     const std::vector<std::pair<size_t, TimeInterval>>& effective_intervals);
    RandomGenerator rng_;
};