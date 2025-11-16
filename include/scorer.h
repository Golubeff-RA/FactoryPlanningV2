#pragma once
#include <concepts>

#include "problem_data.h"
template <typename T>
concept CanScore =
    requires(const ProblemData& data, double not_appointed_coef) {
        { T::CalcScore(data, not_appointed_coef) } -> std::same_as<Score>;
    };

class BasicScorer {
public:
    static Score CalcScore(const ProblemData& data, double not_appointed_coef);
};

class VolumeScorer {
    static Score CalcScore(const ProblemData& data, double not_appointed_coef);
};
