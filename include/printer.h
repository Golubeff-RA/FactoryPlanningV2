#pragma once
#include <format>

#include "defines.h"
#include "problem_data.h"

std::string TimePointToStr(TimePoint tp);

std::string DurationToStr(Duration dur);

class Printer {
public:
    static void PrintGants(const ProblemData& data, std::ostream& out);
    static void PrintShedules(const ProblemData& data, std::ostream& out);
    static void PrintOperations(const ProblemData& data, std::ostream& out);
    static void PrintAnswerJSON(const ProblemData& data, std::ostream& out);
};