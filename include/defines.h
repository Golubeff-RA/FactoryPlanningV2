#pragma once
#include <chrono>
#include <set>
#include <list>
#include <utility>

namespace ch = std::chrono;

using Duration = ch::system_clock::duration;
using TimePoint = ch::system_clock::time_point;
using IdsSet = std::set<size_t>;
using StEndTimes = std::pair<TimePoint, TimePoint>;

inline TimePoint START_TIME_POINT{ch::time_point<ch::system_clock>::min()};