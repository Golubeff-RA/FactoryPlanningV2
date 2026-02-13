#pragma once

#include "basics/problem_data.h"
#include "defines.h"

class BasicGenerator {
public:
    ProblemData Generate(GenerationParams params) {}

protected:
    Duration GetRandomDuration(int min, int max) {
        return Duration{ch::seconds{rng_.GetInt(min, max)}};
    }

    void CreateShedules(GenerationParams params, ProblemData& data);

    bool CanEdgeBeCreated(size_t master_id, size_t slave_id,
                          const ProblemData& data);
    RandomGenerator rng_;
};