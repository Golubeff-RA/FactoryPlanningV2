#pragma once
#include <format>

#include "defines.h"
#include "problem_data.h"

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

class Printer {
public:
    static void PrintGants(const ProblemData& data) {
        for (size_t idx = 0; idx < data.tools.size(); ++idx) {
            std::cout << idx << ") \n";
            for (const auto& interval : data.tools[idx].GetWorkProcess()) {
                std::cout << "(" << interval.operation_id() << ", "
                          << TimePointToStr(interval.start()) << ", "
                          << TimePointToStr(interval.end()) << ")\n";
            }
            std::cout << '\n';
        }
    }

    static void PrintShedules(const ProblemData& data) {
        for (size_t idx = 0; idx < data.tools.size(); ++idx) {
            std::cout << idx << ")\n";
            for (const auto& interval : data.tools[idx].GetShedule()) {
                std::cout << "(" << TimePointToStr(interval.start()) << ", "
                          << TimePointToStr(interval.end()) << ")\n";
            }
            std::cout << '\n';
        }
    }

    static void PrintOperations(const ProblemData& data) {
        for (const auto& operation : data.operations) {
            std::cout << operation.id() << ") " << operation.stoppable()
                      << " start: "
                      << TimePointToStr(operation.GetStEndTimes().first)
                      << " end: " << (TimePointToStr(operation.GetStEndTimes().second)) 
                      << "Appointed: " << operation.Appointed() << "\n";
        }
    }

private:
};