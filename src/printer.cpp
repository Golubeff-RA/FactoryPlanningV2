#include "printer.h"

#include <nlohmann/json.hpp>

using Json = nlohmann::json;

std::string TimePointToStr(TimePoint tp) {
    std::time_t time = ch::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d.%m.%y_%H:%M:%S");
    return oss.str();
}

std::string DurationToStr(Duration dur) {
    auto days = ch::duration_cast<ch::days>(dur);
    auto hours = ch::duration_cast<ch::hours>(dur - days);
    auto minutes = ch::duration_cast<ch::minutes>(dur - days - hours);
    auto seconds = ch::duration_cast<ch::seconds>(dur - days - hours - minutes);

    std::ostringstream oss;
    oss << days.count() << '_' << std::setfill('0') << std::setw(2)
        << hours.count() << ':' << std::setw(2) << minutes.count() << ':'
        << std::setw(2) << seconds.count();

    return oss.str();
}

void Printer::PrintProblemData(const ProblemData& data, std::ostream& out) {
    out << "TOOLS\n" << data.tools.size() << std::endl;

    for (const auto& tool : data.tools) {
        out << "i";
        for (const auto& interval : tool.GetShedule()) {
            out << '(' << TimePointToStr(interval.start()) << ' '
                << TimePointToStr(interval.end()) << ") ";
        }
        out << '\n';
    }

    out << "OPERATIONS\n" << data.operations.size() << std::endl;
    for (const auto& op : data.operations) {
        out << op.stoppable() << ' ';
    }

    out << "\nTIMES\n";
    for (size_t i = 0; i < data.times_matrix.size(); ++i) {
        for (size_t j = 0; j < data.times_matrix[i].size(); ++j) {
            out << DurationToStr(data.times_matrix[i][j]) << ' ';
        }
        out << std::endl;
    }

    for (const auto& work : data.works) {
        size_t cnt_edges = 0;
        for (size_t op_id : work.operation_ids()) {
            cnt_edges += data.operations[op_id].cnt_deps();
        }

        out << "\nwork\n"
            << TimePointToStr(work.start_time()) << ' '
            << TimePointToStr(work.directive()) << ' ' << work.fine_coef()
            << ' ' << work.id() << ' ' << cnt_edges << ' '
            << work.operation_ids().size() << std::endl;
        for (size_t op_id : work.operation_ids()) {
            out << op_id << ' ';
        }
        out << std::endl;
        for (size_t op_id : work.operation_ids()) {
            for (size_t dep_id : data.operations[op_id].depended()) {
                out << op_id << ' ' << dep_id << std::endl;
            }
        }
    }
}

void Printer::PrintGants(const ProblemData& data, std::ostream& out) {
    for (size_t idx = 0; idx < data.tools.size(); ++idx) {
        out << idx << ") \n";
        for (const auto& interval : data.tools[idx].GetWorkProcess()) {
            out << "(" << interval.operation_id() << ", "
                << TimePointToStr(interval.start()) << ", "
                << TimePointToStr(interval.end()) << ")\n";
        }
        out << '\n';
    }
}

void Printer::PrintShedules(const ProblemData& data, std::ostream& out) {
    for (size_t idx = 0; idx < data.tools.size(); ++idx) {
        out << idx << ")\n";
        for (const auto& interval : data.tools[idx].GetShedule()) {
            out << "(" << TimePointToStr(interval.start()) << ", "
                << TimePointToStr(interval.end()) << ")\n";
        }
        out << '\n';
    }
}

void Printer::PrintOperations(const ProblemData& data, std::ostream& out) {
    for (const auto& operation : data.operations) {
        out << operation.id() << ") " << operation.stoppable()
            << " start: " << TimePointToStr(operation.GetStEndTimes().first)
            << " end: " << (TimePointToStr(operation.GetStEndTimes().second))
            << " Appointed: " << operation.Appointed() << "\n";
    }
}

void Printer::PrintWorks(const ProblemData& data, std::ostream& out) {
    for (const auto& work : data.works) {
        out << "id: " << work.id() << " start: " << work.start_time()
            << " directive: " << work.directive()
            << " fine: " << work.fine_coef() << " operations: ";
        for (size_t op_id : work.operation_ids()) {
            out << op_id << " ";
        }
        out << std::endl;
    }
}

Json ToolToJson(const Tool& tool, const ProblemData& data, size_t id) {
    Json works_json = Json::array();
    for (const auto& work_interval : tool.GetWorkProcess()) {
        auto op_id = work_interval.operation_id();
        auto wo_id = data.operations[op_id].ptr_to_work()->id();
        works_json.push_back({{"work", wo_id},
                              {"operation", op_id},
                              {"start", TimePointToStr(work_interval.start())},
                              {"end", TimePointToStr(work_interval.end())}});
    }

    return {{"id", id}, {"work_process", works_json}};
}

Json IdsSetToJson(const IdsSet& ids) {
    Json answer = Json::array();
    for (size_t id : ids) {
        answer.push_back(id);
    }
    return answer;
}

void Printer::PrintAnswerJSON(const ProblemData& data, std::ostream& out) {
    Json answer;
    for (size_t i = 0; i < data.tools.size(); ++i) {
        answer["tools"].push_back({ToolToJson(data.tools[i], data, i)});
    }

    for (const auto& oper : data.operations) {
        answer["operations"].push_back(
            {{"id", oper.id()},
             {"stoppable", oper.stoppable()},
             {"appointed", oper.Appointed()},
             {"start", TimePointToStr(oper.GetStEndTimes().first)},
             {"end", TimePointToStr(oper.GetStEndTimes().second)}});
    }

    for (const auto& work : data.works) {
        answer["works"].push_back(
            {{"id", work.id()},
             {"fine_coef", work.fine_coef()},
             {"operations", IdsSetToJson(work.operation_ids())}});
    }
    out << answer.dump(4);
}
