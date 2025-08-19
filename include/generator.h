#include <random>

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

    std::mt19937 GetGen() {
        return gen;
    }

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
                // с некоторой вероятностью выбираем интервал для создания операции-работы
                // заодно и заполняем матрицу времен Эффективное = длине_интервала
                // остальные = 2 * длина_интервала
                if (rng.GetBool(0.5)) {
                    data.works.push_back(
                        Work(start_shed, end, 0.5, data.works.size()));
                    data.works.back().AddOperation(data.operations.size());
                    data.operations.push_back(
                        Operation(data.operations.size(), false));

                    data.times_matrix.push_back(std::vector<Duration>(
                        cnt_tools, (end - start_shed) * 2));
                    data.times_matrix.back()[i] =
                        end - start_shed;

                    
                }
                start_shed = end + GetRandomDuration(100, 1000);
            }
            data.tools.push_back(Tool(shedule));
        }
    
        size_t cnt_new_deps = 1;
        while (cnt_new_deps != 0) {
            cnt_new_deps = 0;
            std::shuffle(data.works.begin(), data.works.end(), rng.GetGen());
            for (auto& work_master : data.works) {
                if (work_master.operation_ids().size() == 0) {
                    continue;
                }
                for (auto& work_slave: data.works) {
                    // работу саму с собой склеивать нельзя.
                    if (work_master.id() == work_slave.id() || work_slave.operation_ids().size() != 1) {
                        continue;
                    }

                    

                }
            }
        }
        return data;
    }

private:
    Duration GetRandomDuration(int min, int max) {
        return Duration{ch::seconds{rng.GetInt(min, max)}};
    }

    RandomGenerator rng;
};