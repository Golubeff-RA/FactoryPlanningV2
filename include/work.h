#pragma once
#include "defines.h"

class Work {
public:
    Work(TimePoint start, TimePoint directive, double fine_coef, size_t id);

    TimePoint start_time() const;

    TimePoint directive() const;

    double fine_coef() const;

    size_t id() const;

    bool AddOperation(size_t id);

    const IdsSet& operation_ids() const;

private:
    TimePoint start_time_;
    TimePoint directive_;
    double fine_coef_;
    size_t id_;
    IdsSet operation_ids_;
};