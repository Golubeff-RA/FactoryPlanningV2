#include "problem_data.h"

#include <regex>

TimePoint ParseTimePoint(const std::string& str) {
    std::istringstream iss(str);
    std::tm tm = {};
    iss >> std::get_time(&tm, "%d.%m.%y_%H:%M:%S");

    if (iss.fail()) {
        throw std::runtime_error("Ошибка парсинга времени " + str);
    }

    std::time_t time = std::mktime(&tm);
    return ch::system_clock::from_time_t(time);
};

Duration ParseDuration(const std::string& str) {
    std::regex dur_regex(R"((\d+)_(\d+):(\d+):(\d+))");
    std::smatch matches;

    if (std::regex_match(str, matches, dur_regex)) {
        int days = stoi(matches[1]);
        int hours = stoi(matches[2]);
        int minutes = stoi(matches[3]);
        int seconds = stoi(matches[4]);

        return ch::days(days) + ch::hours(hours) + ch::minutes(minutes) +
               ch::seconds(seconds);
    }

    throw std::runtime_error("Неверный формат Duration: " + str);
    return Duration(0);
};

ProblemData::ProblemData(std::ifstream& input) {
    std::string buffer;
    size_t counter = 0;
    while (!input.eof() && counter != 100) {
        std::getline(input, buffer);
        ++counter;
        if (buffer == "TOOLS") {
            FillTools(input);
        } else if (buffer == "OPERATIONS") {
            FillOperations(input);
        } else if (buffer == "TIMES") {
            FillTimes(input);
        } else if (buffer == "work") {
            AddWork(input);
        }
    }

    for (size_t i = 0; i < works.size(); ++i) {
        for (size_t op_id : works[i]->operation_ids()) {
            operations[op_id].SetWorkPtr((works[i]));
        }
    }
}

void ProblemData::FillTools(std::ifstream& input) {
    std::string buf;
    std::getline(input, buf);
    size_t cnt_tools = std::stoul(buf);
    tools.reserve(cnt_tools);
    for (size_t idx = 0; idx < cnt_tools && !input.eof(); ++idx) {
        tools.push_back(ParseTool(input, idx));
    }
}

Tool ProblemData::ParseTool(std::ifstream& input, size_t idx) {
    std::set<TimeInterval> shedule;
    std::string buffer;
    std::getline(input, buffer);
    std::regex time_regex(R"((\d{2}\.\d{2}\.\d{2}_\d{2}:\d{2}:\d{2}))");
    std::smatch matches;

    std::string::const_iterator search_start(buffer.cbegin());
    std::vector<std::string> time_strs;
    while (
        std::regex_search(search_start, buffer.cend(), matches, time_regex)) {
        time_strs.push_back(matches[1]);
        search_start = matches[0].second;
    }

    for (size_t i = 0; i + 1 < time_strs.size(); i += 2) {
        const std::string& start_str = time_strs[i];
        const std::string& end_str = time_strs[i + 1];
        TimePoint start = ParseTimePoint(start_str);
        TimePoint end = ParseTimePoint(end_str);
        shedule.insert({start, end});
    }
    return Tool{shedule};
}

void ProblemData::FillOperations(std::ifstream& input) {
    size_t cnt_operations;
    bool stoppable;
    input >> cnt_operations;
    operations.reserve(cnt_operations);
    for (size_t idx = 0; idx < cnt_operations && !input.eof(); ++idx) {
        input >> stoppable;
        operations.push_back(Operation(idx, stoppable));
    }
}

void ProblemData::FillTimes(std::ifstream& input) {
    times_matrix.resize(operations.size());
    for (size_t i = 0; i < operations.size() && !input.eof(); ++i) {
        times_matrix[i].resize(tools.size());
        for (size_t j = 0; j < tools.size() && !input.eof(); ++j) {
            std::string buff;
            input >> buff;
            times_matrix[i][j] = ParseDuration(buff);
            if (times_matrix[i][j] != Duration(0)) {
                operations[i].AddPossibleTool(j);
            }
        }
    }
}

void ProblemData::AddWork(std::ifstream& input) {
    std::string start_str, end_str;
    size_t id;
    double fine_coef;
    size_t cnt_edges;
    size_t cnt_ops;
    input >> start_str >> end_str >> fine_coef >> id >> cnt_edges >> cnt_ops;
    TimePoint start = ParseTimePoint(start_str);
    TimePoint directive = ParseTimePoint(end_str);
    works.push_back(WorkPtr(new Work(start, directive, fine_coef, id)));
    size_t op_id;
    for (size_t i = 0; i < cnt_ops && !input.eof(); ++i) {
        input >> op_id;
        works.back()->AddOperation(op_id);
    }
    size_t master, slave;
    for (size_t i = 0; i < cnt_edges && !input.eof(); ++i) {
        input >> master >> slave;
        operations[master].AddDepended(slave);
        operations[slave].AddDependency(master);
    }
}