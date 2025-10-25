#include <scorer.h>

Score BasicScorer::CalcScore(const ProblemData& data, double not_appointed_coef) {
    Score score;
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

    for (WorkPtr work : fully_appointed) {
        TimePoint end_time = START_TIME_POINT;
        for (size_t id : work->operation_ids()) {
            end_time = std::max(data.operations[id].GetStEndTimes().second, end_time);
        }
        score.appointed_fine +=
            work->fine_coef() *
            std::chrono::duration<double>(std::max(end_time - work->directive(), Duration(0)))
                .count();
    }

    for (WorkPtr work : not_fully_appointed) {
        for (size_t id : work->operation_ids()) {
            if (!data.operations[id].Appointed()) {
                score.not_appointed_fine += work->fine_coef() * not_appointed_coef;
            }
        }
    }

    return score;
}