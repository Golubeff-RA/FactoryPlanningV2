#include "basics/work.h"

Work::Work(TimePoint start, TimePoint directive, double fine_coef, size_t id)
    : start_time_(start),
      directive_(directive),
      fine_coef_(fine_coef),
      id_(id) {}

TimePoint Work::start_time() const { return start_time_; }

TimePoint Work::directive() const { return directive_; }

double Work::fine_coef() const { return fine_coef_; }

size_t Work::id() const { return id_; }

bool Work::AddOperation(size_t id) { return operation_ids_.insert(id).second; }
bool Work::DelOperation(size_t id) { return operation_ids_.erase(id) != 0U; }

const IdsSet& Work::operation_ids() const { return operation_ids_; }
