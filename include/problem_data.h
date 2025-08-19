#pragma once
#include <fstream>
#include <string>

#include "operation.h"
#include "tool.h"
#include "work.h"

TimePoint ParseTimePoint(const std::string& str);

Duration ParseDuration(const std::string& str);

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
