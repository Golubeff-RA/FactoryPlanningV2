#pragma once
#include "defines.h"

class Work {
public:
    Work(TimePoint start, TimePoint directive, double fine_coef);

    TimePoint start_time() const;

    TimePoint directive() const;

    double fine_coef() const;

    bool AddOperation(size_t id);

    const IdsSet& operation_ids() const;
private:
    TimePoint start_time_;
    TimePoint directive_;
    double fine_coef_;
    IdsSet operation_ids_;
};