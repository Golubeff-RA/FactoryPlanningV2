#include <scorer.h>

#include "operation.h"
#include "problem_data.h"
#include "work.h"

std::pair<std::vector<WorkPtr>, std::vector<WorkPtr>> SeparateWorks(
    const ProblemData& data) {
    std::vector<WorkPtr> fully_appointed;
    std::vector<WorkPtr> not_fully_appointed;
    for (WorkPtr work : data.works) {
        bool flag = true;
        for (size_t id : work->operation_ids()) {
            if (!data.operations[id].Appointed()) {
                flag = false;
                break;
            }
        }
        if (flag) {
            fully_appointed.push_back(work);
        } else {
            not_fully_appointed.push_back(work);
        }
    }

    return {fully_appointed, not_fully_appointed};
}

double CalcNotAppointedFine(const ProblemData& data,
                            const std::vector<WorkPtr> not_fully_appointed,
                            double not_appointed_coef) {
    double answer = 0;
    for (WorkPtr work : not_fully_appointed) {
        for (size_t id : work->operation_ids()) {
            if (!data.operations[id].Appointed()) {
                answer += work->fine_coef() * not_appointed_coef;
            }
        }
    }

    return answer;
}

Score BasicScorer::CalcScore(const ProblemData& data,
                             double not_appointed_coef) {
    Score score;
    auto [fully_appointed, not_fully_appointed] = SeparateWorks(data);

    for (WorkPtr work : fully_appointed) {
        TimePoint end_time = START_TIME_POINT;
        for (size_t id : work->operation_ids()) {
            end_time =
                std::max(data.operations[id].GetStEndTimes().second, end_time);
        }
        score.appointed_fine +=
            work->fine_coef() *
            std::chrono::duration<double>(
                std::max(end_time - work->directive(), Duration(0)))
                .count();
    }

    score.not_appointed_fine =
        CalcNotAppointedFine(data, not_fully_appointed, not_appointed_coef);
    return score;
}

Score VolumeScorer::CalcScore(const ProblemData& data,
                              double not_appointed_coef) {
    Score score;
    auto [fully_appointed, not_fully_appointed] = SeparateWorks(data);

    for (WorkPtr work : fully_appointed) {
        for (size_t id : work->operation_ids()) {
            const Operation& operation(data.operations[id]);
            score.appointed_fine +=
                work->fine_coef() *
                std::chrono::duration<double>(
                    std::max(
                        operation.GetStEndTimes().second - work->directive(),
                        Duration(0)))
                    .count();
        }
    }

    score.not_appointed_fine =
        CalcNotAppointedFine(data, not_fully_appointed, not_appointed_coef);

    return score;
};