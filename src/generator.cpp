#include "generator.h"

#include <algorithm>
#include <queue>

ProblemData Generator::Generate(GenerationParams params) {
    rng_ = RandomGenerator(params.seed);
    ProblemData data;
    // создаём расписание исполнителей
    CreateShedules(params, data);
    // создание работ и операций (id_исполнителя, интервал)
    std::vector<std::pair<size_t, TimeInterval>> effective_intervals;
    CreateOperations(params, data, effective_intervals);
    // добавление зависимостей в работы
    CreateEdges(params, data, effective_intervals);

    std::vector<WorkPtr> works_not_empty;
    for (auto& ptr : data.works) {
        if (!ptr->operation_ids().empty()) {
            works_not_empty.push_back(ptr);
        }
    }

    data.works = works_not_empty;

    double sum = 0;
    std::vector<double> fines(data.works.size(), 0.0);
    for (auto& fine : fines) {
        fine = rng_.GetDouble(0.0, 1.0);
        sum += fine;
    }

    for (size_t i = 0; i < data.works.size(); ++i) {
        data.works[i]->fine_coef_ = fines[i] / sum;
    }
    return data;
}

bool Generator::CanEdgeBeCreated(
    size_t master_id, size_t slave_id,
    const std::vector<std::pair<size_t, TimeInterval>>& effective_intervals,
    const ProblemData& data) {
    if (data.operations[master_id].ptr_to_work() !=
            data.operations[slave_id].ptr_to_work() &&
        (!data.operations[slave_id].dependencies().empty() ||
         !data.operations[slave_id].depended().empty())) {
        return false;
    }
    if (data.operations[master_id].depended().contains(slave_id) ||
        data.operations[slave_id].dependencies().contains(master_id)) {
        return false;
    }
    // саму с собой связывать тоже нельзя
    if (master_id == slave_id) {
        return false;
    }

    std::vector<bool> visited(data.operations.size(), false);
    std::queue<size_t> que;
    que.push(slave_id);
    visited[slave_id] = true;
    while (!que.empty()) {
        size_t current = que.front();
        que.pop();
        // мы создадим цикл
        if (current == master_id) {
            return false;
        }

        for (int neighbor : data.operations[current].depended()) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                que.push(neighbor);
            }
        }
    }

    // операции эффективно выполняются на 1 исполнителе
    if (effective_intervals[master_id].first ==
        effective_intervals[slave_id].first) {
        // эффективные интервалы выполнения пересекаются - нельзя
        if (effective_intervals[master_id].second.Intersects(
                effective_intervals[slave_id].second)) {
            return false;
        }
    } else {
        // slave эффективно начинается раньше master - нельзя
        if (effective_intervals[master_id].second.start() >
            effective_intervals[slave_id].second.start()) {
            return false;
        }
    }

    return true;
}

void Generator::CreateShedules(GenerationParams params, ProblemData& data) {
    for (size_t i = 0; i < params.cnt_tools; ++i) {
        std::set<TimeInterval> shedule;
        TimePoint start_shed =
            params.start + GetRandomDuration(params.spacer_dur.first,
                                             params.spacer_dur.second);
        for (size_t j = 0; j < params.interval_per_tool * 2; ++j) {
            TimePoint end =
                start_shed + GetRandomDuration(params.interval_dur.first,
                                               params.interval_dur.second);
            shedule.insert(TimeInterval(start_shed, end));
            start_shed = end + GetRandomDuration(params.spacer_dur.first,
                                                 params.spacer_dur.second);
        }
        data.tools.push_back(Tool(shedule));
    }
}

constexpr size_t kWorkIdMult = 100;

void Generator::CreateOperations(
    GenerationParams params, ProblemData& data,
    std::vector<std::pair<size_t, TimeInterval>>& effective_intervals) {
    for (size_t i = 0; i < data.tools.size(); ++i) {
        auto tool = data.tools[i];
        size_t counter = 0;
        for (auto it = tool.GetShedule().begin();
             it != tool.GetShedule().end() &&
             counter < tool.GetShedule().size() / 2;
             ++it, ++counter) {
            auto interval = TimeInterval(
                it->start(), it->start() + (it->end() - it->start()) / 5 * 4);

            if (rng_.GetBool(params.operation_create_prob)) {
                Operation op{data.operations.size(), rng_.GetBool()};
                // добавим возможных исполнителей
                for (size_t j = 0; j < data.tools.size(); ++j) {
                    if (rng_.GetBool(params.add_tool_prob)) {
                        op.AddPossibleTool(j);
                    }
                }
                op.AddPossibleTool(i);

                data.works.push_back(
                    WorkPtr(new Work{interval.start(), interval.end(), 0,
                                     (data.works.size() + 1) * kWorkIdMult}));
                op.SetWorkPtr(data.works.back());
                data.operations.push_back(op);

                data.times_matrix.push_back(
                    std::vector<Duration>(params.cnt_tools));
                for (size_t j = 0; j < params.cnt_tools; ++j) {
                    if (op.possible_tools().contains(j)) {
                        data.times_matrix.back()[j] =
                            GetRandomDuration(ch::duration_cast<ch::seconds>(
                                                  interval.GetTimeSpan())
                                                      .count() +
                                                  2,
                                              ch::duration_cast<ch::seconds>(
                                                  interval.GetTimeSpan())
                                                      .count() *
                                                  4);
                    } else {
                        data.times_matrix.back()[j] = Duration(0);
                    }
                }

                data.times_matrix.back()[i] = interval.GetTimeSpan();
                effective_intervals.push_back({i, interval});
            }
        }
    }

    std::vector<size_t> permutation(data.operations.size(), 0);
    std::iota(permutation.begin(), permutation.end(), 0);
    std::shuffle(permutation.begin(), permutation.end(), rng_.GetGen());

    std::vector<Operation> ops;
    ops.reserve(data.operations.size());
    std::vector<std::pair<size_t, TimeInterval>> eff;
    eff.reserve(data.operations.size());

    std::transform(permutation.begin(), permutation.end(),
                   std::back_inserter(ops),
                   [&](size_t idx) { return data.operations[idx]; });

    std::transform(permutation.begin(), permutation.end(),
                   std::back_inserter(eff),
                   [&](size_t idx) { return effective_intervals[idx]; });
    effective_intervals = eff;
}

void Generator::CreateEdges(
    GenerationParams params, ProblemData& data,
    const std::vector<std::pair<size_t, TimeInterval>>& effective_intervals) {
    size_t cnt_new_deps = 1;
    // порядок обхода операций
    std::vector<size_t> numeration(data.operations.size(), 0);
    std::iota(numeration.begin(), numeration.end(), 0);
    while (cnt_new_deps != 0) {
        cnt_new_deps = 0;
        std::shuffle(numeration.begin(), numeration.end(), rng_.GetGen());
        for (size_t master_op_id : numeration) {
            for (size_t slave_op_id : numeration) {
                // мы можем соединить операции
                if (CanEdgeBeCreated(master_op_id, slave_op_id,
                                     effective_intervals, data) &&
                    rng_.GetBool(params.edge_create_prob)) {
                    ++cnt_new_deps;
                    data.operations[master_op_id].AddDepended(
                        data.operations[slave_op_id].id());
                    data.operations[slave_op_id].AddDependency(
                        data.operations[master_op_id].id());
                    if (data.operations[slave_op_id].ptr_to_work() !=
                        data.operations[master_op_id].ptr_to_work()) {
                        data.operations[slave_op_id]
                            .ptr_to_work()
                            ->DelOperation(data.operations[slave_op_id].id());
                        data.operations[slave_op_id].SetWorkPtr(
                            data.operations[master_op_id].ptr_to_work());
                    }
                    // обновляем время начала работы для работы по добавленной
                    // операции
                    data.operations[master_op_id].ptr_to_work()->start_time_ =
                        std::min(
                            data.operations[master_op_id]
                                .ptr_to_work()
                                ->start_time_,
                            effective_intervals[slave_op_id].second.start());
                    // обновляем директивный срок для работы по добавленной
                    // операции
                    data.operations[master_op_id]
                        .ptr_to_work()
                        ->directive_ = std::max(
                        data.operations[master_op_id].ptr_to_work()->directive_,
                        effective_intervals[slave_op_id].second.end());

                    break;
                }
            }
        }
    }

    std::sort(data.operations.begin(), data.operations.end(),
              [&](const Operation& first, const Operation& second) {
                  return first.id() < second.id();
              });
}