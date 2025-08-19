#include "solver.h"

void Solver::Solve(ProblemData& data) {
    TimePoint current_time;
    std::set<TimePoint> timestamps = GetStartTimes(data);

    // собираем начальный фронт
    /*IdsSet F;
    for (const auto& op : data.operations) {
        if (op.CanBeAppointed(*timestamps.begin())) {
            F.insert(op.id());
        }
    }*/

    while (!timestamps.empty()) {
        IdsSet R;
        IdsSet F;
        current_time = *timestamps.begin();
        timestamps.erase(timestamps.begin());

        // вот тут по идее надо по-умному собирать фронт на основе
        // зависимостей хранящихся в операциях, но я хлебушек и не знаю
        for (const auto& operation : data.operations) {
            if (operation.CanBeAppointed(current_time)) {
                F.insert(operation.id());
            }
        }
        // SortFront(data, F);
        //  сборка возможных исполнителей
        for (size_t oper : F) {
            for (size_t i : data.operations[oper].possible_tools()) {
                if (data.tools[i].CanStartWork(data.operations[oper],
                                               current_time,
                                               data.times_matrix[oper][i])) {
                    R.insert(i);
                }
            }
        }

        // назначение
        for (size_t oper : F) {
            auto& operation = data.operations[oper];
            for (size_t r : R) {
                if (operation.possible_tools().contains(r) &&
                    data.tools[r].CanStartWork(operation, current_time,
                                               data.times_matrix[oper][r])) {
                    data.tools[r].Appoint(operation, current_time,
                                          data.times_matrix[oper][r],
                                          data.operations);
                    timestamps.insert(operation.GetStEndTimes().second);
                    R.erase(r);
                    break;
                }
            }
        }
    }
}

std::set<TimePoint> Solver::GetStartTimes(const ProblemData& data) {
    std::set<TimePoint> times;
    for (const auto& work : data.works) {
        times.insert(work.start_time());
    }

    for (const auto& tool : data.tools) {
        for (const auto& interval : tool.GetShedule()) {
            times.insert(interval.start());
        }
    }

    return times;
}