#include <map>
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
    ProblemData Generate(size_t cnt_tools, size_t cnt_intervals,
                         TimePoint start, int seed) {
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

        // создание работ и операций
        std::vector<TimeInterval> effective_intervals;
        for (size_t i = 0; i < data.tools.size(); ++i) {
            auto tool = data.tools[i];
            for (const auto& interval : tool.GetShedule()) {
                if (rng.GetBool(0.4)) {
                    Operation op{data.operations.size(), true};
                    Work work{interval.start(), interval.end(), 0,
                              data.works.size()};
                    op.SetWorkPtr(&work);
                    // добавим возможных исполнителей
                    for (size_t j = 0; j < data.tools.size(); ++j) {
                        op.AddPossibleTool(j);
                    }

                    data.works.push_back(std::move(work));
                    data.operations.push_back(op);

                    data.times_matrix.push_back(std::vector<Duration>(
                        cnt_tools, interval.GetTimeSpan() * 2));
                    data.times_matrix.back()[i] = interval.GetTimeSpan();
                    effective_intervals.push_back(interval);
                }
            }
        }


        // добавление зависимостей в работы
        size_t cnt_new_deps = 1;
        
        return data;
    }

private:
    Duration GetRandomDuration(int min, int max) {
        return Duration{ch::seconds{rng.GetInt(min, max)}};
    }

    // проверяет возможность установки связи между операциями master_id -> slave_id
    bool CanEdgeBeCreated(size_t master_id, size_t slave_id,
                          const std::vector<TimeInterval> effective_intervals,
                          const ProblemData& data) const {
        // если они уже связаны, то нельзя
        if (data.operations[master_id].depended().contains(slave_id) ||
            data.operations[slave_id].depended().contains(master_id)) {
            return false;
        }

        // саму с собой связывать тоже нельзя
        if (master_id == slave_id) {
            return false;
        }

        // slave эффективно начинается раньше master - нельзя
        if (effective_intervals[master_id].start() > effective_intervals[slave_id].start()) {
            return false;
        }

        // эффективные интервалы выполнения пересекаются - нельзя
        if (effective_intervals[master_id].Intersects(effective_intervals[slave_id])) {
            return false;
        }

        return true;
    }

    RandomGenerator rng;
};