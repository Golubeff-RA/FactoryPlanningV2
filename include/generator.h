#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <vector>

#include "problem_data.h"
class RandomGenerator {
public:
    RandomGenerator(int seed) : gen(std::mt19937(seed)) {}

    int GetInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }

    double GetDouble(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(gen);
    }

    bool GetBool(double prob = 0.5) {
        std::bernoulli_distribution dist(prob);
        return dist(gen);
    }

    std::mt19937 GetGen() { return gen; }

private:
    std::mt19937 gen;
};

class Generator {
public:
    Generator() : rng(52) {}
    ProblemData Generate(size_t cnt_tools, size_t cnt_intervals, TimePoint start, int seed) {
        rng = RandomGenerator(seed);
        ProblemData data;

        // создаём расписание исполнителей
        for (size_t i = 0; i < cnt_tools; ++i) {
            std::set<TimeInterval> shedule;
            TimePoint start_shed = start + GetRandomDuration(100, 1000);
            for (size_t j = 0; j < cnt_intervals; ++j) {
                TimePoint end = start_shed + GetRandomDuration(100, 1000);
                shedule.insert(TimeInterval(start_shed, end));
                start_shed = end + GetRandomDuration(100, 1000);
            }
            data.tools.push_back(Tool(shedule));
        }

        // создание работ и операций (id_исполнителя, интервал)
        std::vector<std::pair<size_t, TimeInterval>> effective_intervals;
        for (size_t i = 0; i < data.tools.size(); ++i) {
            auto tool = data.tools[i];
            for (const auto& interval : tool.GetShedule()) {
                if (rng.GetBool(0.4)) {
                    Operation op{data.operations.size(), true};

                    // добавим возможных исполнителей
                    for (size_t j = 0; j < data.tools.size(); ++j) {
                        op.AddPossibleTool(j);
                    }

                    data.works.push_back(
                        WorkPtr(new Work{interval.start(), interval.end(), 0, data.works.size()}));
                    op.SetWorkPtr(data.works.back());
                    data.operations.push_back(op);

                    data.times_matrix.push_back(
                        std::vector<Duration>(cnt_tools, interval.GetTimeSpan() * 2));
                    data.times_matrix.back()[i] = interval.GetTimeSpan();
                    effective_intervals.push_back({i, interval});
                }
            }
        }

        // добавление зависимостей в работы
        size_t cnt_new_deps = 1;
        // порядок обхода операций
        std::vector<size_t> numeration(data.operations.size(), 0);
        std::iota(numeration.begin(), numeration.end(), 0);
        while (cnt_new_deps != 0) {
            cnt_new_deps = 0;
            std::shuffle(numeration.begin(), numeration.end(), rng.GetGen());
            for (size_t master_op_id : numeration) {
                for (size_t slave_op_id : numeration) {
                    // мы можем соединить операции
                    if (CanEdgeBeCreated(master_op_id, slave_op_id, effective_intervals, data)) {
                        ++cnt_new_deps;
                        // std::cout << "New edge: " << master_op_id << ' ' << slave_op_id
                        //           << std::endl;
                        data.operations[master_op_id].AddDepended(slave_op_id);
                        data.operations[slave_op_id].AddDependency(master_op_id);
                        if (data.operations[slave_op_id].ptr_to_work() !=
                            data.operations[master_op_id].ptr_to_work()) {
                            data.operations[slave_op_id].ptr_to_work()->DelOperation(slave_op_id);
                            data.operations[slave_op_id].SetWorkPtr(
                                data.operations[master_op_id].ptr_to_work());
                        }
                        // обновляем время начала работы для работы по добавленной операции
                        data.operations[master_op_id].ptr_to_work()->start_time_ =
                            std::min(data.operations[master_op_id].ptr_to_work()->start_time_,
                                     effective_intervals[slave_op_id].second.start());
                        // обновляем директивный срок для работы по добавленной операции
                        data.operations[master_op_id].ptr_to_work()->directive_ =
                            std::max(data.operations[master_op_id].ptr_to_work()->directive_,
                                     effective_intervals[slave_op_id].second.end());

                        break;
                    }
                }
            }
        }

        std::vector<WorkPtr> works_not_empty;
        for (auto& ptr : data.works) {
            if (!ptr->operation_ids().empty()) {
                works_not_empty.push_back(ptr);
            }
        }

        data.works = works_not_empty;
        return data;
    }

private:
    Duration GetRandomDuration(int min, int max) {
        return Duration{ch::seconds{rng.GetInt(min, max)}};
    }

    // проверяет возможность установки связи между операциями master_id ->
    // slave_id
    bool CanEdgeBeCreated(size_t master_id, size_t slave_id,
                          const std::vector<std::pair<size_t, TimeInterval>>& effective_intervals,
                          const ProblemData& data) const {
        if (data.operations[master_id].ptr_to_work() != data.operations[slave_id].ptr_to_work() &&
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
        std::queue<size_t> q;
        q.push(slave_id);
        visited[slave_id] = true;
        while (!q.empty()) {
            size_t current = q.front();
            q.pop();
            // мы создадим цикл
            if (current == master_id) {
                // std::cout << "Cycle dedected\n";
                return false;
            }

            for (int neighbor : data.operations[current].depended()) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }

        // операции эффективно выполняются на 1 исполнителе
        if (effective_intervals[master_id].first == effective_intervals[slave_id].first) {
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

    RandomGenerator rng;
};