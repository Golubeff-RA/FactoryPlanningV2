#pragma once
#include "problem_data.h"

class BasicScorer {
public:
    static Score CalcScore(const ProblemData& data, double not_appointed_coef);
};
