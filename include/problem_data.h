#pragma once
#include <fstream>
#include <string>

#include "operation.h"
#include "tool.h"
#include "work.h"

TimePoint ParseTimePoint(std::istream& in) {
    std::tm tm;
    char sep1, sep2, sep3, sep4, sep5;

    in >> std::get_time(&tm, "%d") >> sep1 >> std::get_time(&tm, "%m") >>
        sep2 >> std::get_time(&tm, "%y") >> sep3 >> std::get_time(&tm, "%H") >>
        sep4 >> std::get_time(&tm, "%M") >> sep5 >> std::get_time(&tm, "S");

    if (in.fail() || sep1 != ':' || sep2 != ':' || sep3 != '_' || sep4 != ':' ||
        sep5 != ':') {
        in.setstate(std::ios::failbit);
        throw std::runtime_error("Error parsing TimePoint");
    }

    std::time_t time = std::mktime(&tm);
    if (time == -1) {
        throw std::runtime_error("Invalid time");
    }

    return ch::system_clock::from_time_t(time);
};

Duration ParseDuration(std::istream& in) {
    uint32_t days = 0, hours = 0, minutes = 0, seconds = 0;
    char sep1, sep2, sep3;

    in >> days >> sep1 >> hours >> sep2 >> minutes >> sep3 >> seconds;
    if (in.fail() || sep1 != '_' || sep2 != ':' || sep3 != ':') {
        in.setstate(std::ios::failbit);
        throw std::runtime_error("Error parsing Duration DD_HH:MM:SS");
    }

    if (hours >= 24 || minutes >= 60 || seconds >= 60) {
        throw std::runtime_error("Invalid time values");
    }

    auto total = ch::days(days) + ch::hours(hours) + ch::minutes(minutes) +
                 ch::seconds(seconds);
    return Duration(total);
};

class ProblemData {
public:
    ProblemData(std::ifstream& input);

    std::vector<Tool> tools;
    std::vector<Work> works;
    std::vector<Operation> operations;
    std::vector<std::vector<Duration>> times_matrix;

private:
    void FillTools(std::ifstream& input);
    void FillOperations(std::ifstream& input);
    void FillTimes(std::ifstream& input);
    void AddWork(std::ifstream& input);

    Tool ParseTool(std::ifstream& input, size_t id);
};

ProblemData::ProblemData(std::ifstream& input) {
    std::string buffer;
    while (!input.eof()) {
        std::getline(input, buffer);

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

    for (Work& work : works) {
        for (size_t op_id : work.operation_ids()) {
            operations[op_id].SetWorkPtr(&work);
        }
    }
}

void ProblemData::FillTools(std::ifstream& input) {
    size_t cnt_tools = 0;
    input >> cnt_tools;
    tools.reserve(cnt_tools);
    for (size_t idx = 0; idx < cnt_tools && !input.eof(); ++idx) {
        tools.push_back(ParseTool(input, idx));
    }
}

Tool ProblemData::ParseTool(std::ifstream& input, size_t idx) {
    std::set<TimeInterval> shedule;
    std::string buffer;
    std::getline(input, buffer);
    std::istringstream iss(buffer);
    char sep;
    TimePoint start, end;
    while (iss >> sep) {
        start = ParseTimePoint(iss);
        end = ParseTimePoint(iss);
        shedule.insert({start, end});
        iss >> sep;
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
        times_matrix[i].resize(tools.size(), Duration(0));
        for (size_t j = 0; j < tools.size() && !input.eof(); ++i) {
            times_matrix[i][j] = ParseDuration(input);
        }
    }
}

void ProblemData::AddWork(std::ifstream& input) {
    TimePoint start = ParseTimePoint(input);
    TimePoint directive = ParseTimePoint(input);
    double fine_coef;
    size_t cnt_edges;
    input >> fine_coef >> cnt_edges;
    works.push_back(Work(start, directive, fine_coef));

    size_t master, slave;
    for (size_t i = 0; i < cnt_edges; ++i) {
        input >> master >> slave;
        works.back().AddOperation(master);
        works.back().AddOperation(slave);
        operations[master].AddDepended(slave);
        operations[slave].AddDependency(master);
    }
}