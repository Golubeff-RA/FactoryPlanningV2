#pragma once
#include "defines.h"

class Work {
public:
    Work(TimePoint start, TimePoint directive, double fine_coef)
        : start_time_(start), directive_(directive), fine_coef_(fine_coef) {}

    TimePoint start_time() const {
        return start_time_;
    }

    TimePoint directive() const {
        return directive_;
    }

    double fine_coef() const {
        return fine_coef_;
    }

    bool AddOperation(size_t id) {
        return operation_ids_.insert(id).second;
    }

    const IdsSet& operation_ids() const {
        return operation_ids_;
    }

private:
    TimePoint start_time_;
    TimePoint directive_;
    double fine_coef_;
    IdsSet operation_ids_;
};