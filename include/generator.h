#pragma once
#include <algorithm>
#include <queue>
#include <ranges>
#include <vector>

#include "problem_data.h"

class Generator {
public:
    Generator() : rng(52) {}
    ProblemData Generate(GenerationParams params);

private:
    Duration GetRandomDuration(int min, int max) {
        return Duration{ch::seconds{rng.GetInt(min, max)}};
    }

    // проверяет возможность установки связи между операциями master_id -> slave_id
    bool CanEdgeBeCreated(size_t master_id, size_t slave_id,
                          const std::vector<std::pair<size_t, TimeInterval>>& effective_intervals,
                          const ProblemData& data) const;

    void CreateShedules(GenerationParams params, ProblemData& data);

    void CreateOperations(GenerationParams params, ProblemData& data,
                          std::vector<std::pair<size_t, TimeInterval>>& effective_intervals);

    void CreateEdges(GenerationParams params, ProblemData& data,
                     const std::vector<std::pair<size_t, TimeInterval>>& effective_intervals);
    RandomGenerator rng;
};