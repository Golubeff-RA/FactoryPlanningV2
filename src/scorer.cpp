#include <scorers/scorer.h>

#include "basics/operation.h"
#include "basics/problem_data.h"
#include "basics/work.h"
#include "defines.h"

std::pair<std::vector<WorkPtr>, std::vector<WorkPtr>> SeparateWorks(
    const ProblemData& data) {
    std::vector<WorkPtr> fully_appointed;
    std::vector<WorkPtr> not_fully_appointed;
    for (WorkPtr work : data.works) {
        bool flag = true;
        for (size_t id : work->OperationIDs()) {
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
        for (size_t id : work->OperationIDs()) {
            if (!data.operations[id].Appointed()) {
                answer += work->FineCoef() * not_appointed_coef;
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
        TimePoint end_time = kStartTimePoint;
        for (size_t id : work->OperationIDs()) {
            end_time =
                std::max(data.operations[id].GetStEndTimes().second, end_time);
        }
        score.appointed_fine +=
            work->FineCoef() *
            std::chrono::duration<double>(
                std::max(end_time - work->Directive(), Duration(0)))
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
        for (size_t id : work->OperationIDs()) {
            const Operation& operation(data.operations[id]);
            score.appointed_fine +=
                work->FineCoef() *
                std::chrono::duration<double>(
                    std::max(
                        operation.GetStEndTimes().second - work->Directive(),
                        Duration(0)))
                    .count();
        }
    }

    score.not_appointed_fine =
        CalcNotAppointedFine(data, not_fully_appointed, not_appointed_coef);

    return score;
};