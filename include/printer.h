#pragma once
#include <format>

#include "defines.h"

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
    oss << days.count() << '_' << std::setfill('0') << std::setw(2) << hours.count()
        << ':' << std::setw(2) << minutes.count() << ':' << std::setw(2)
        << seconds.count();

    return oss.str();
}

class Printer {};