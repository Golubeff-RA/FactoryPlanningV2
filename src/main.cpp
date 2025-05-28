#include <time.h>

#include <fstream>
#include <iostream>
#include <random>

#include "printer.h"
#include "problem_data.h"

using namespace std::chrono_literals;

Duration RndDuration() { return Duration(ch::seconds(rand() % 3000)); }

void WriteTestData() {
    srand(time(0));
    auto now = ch::system_clock::now();
    std::ofstream out("test.txt");
    size_t cnt_tools = rand() % 20;
    size_t cnt_operations = rand() % 40;
    out << "TOOLS\n" << cnt_tools << std::endl;
    

    for (size_t i = 0; i < cnt_tools; ++i) {
        out << "i";
        size_t cnt = rand() % 15 + 2;
        Duration dur(0);
        for (size_t j = 0; j < cnt; ++j) {
            dur += RndDuration();
            out << '(' << TimePointToStr(now + dur) << ' ';
            dur += RndDuration();
            out << TimePointToStr(now + dur) << ") ";
        }
        out << '\n';
    }

    out << "OPERATIONS\n" << cnt_operations << std::endl;
    for (size_t i = 0; i < cnt_operations; ++i) {
        out << rand() % 2 << ' ';
    }

    out << "\nTIMES\n";
    for (size_t i = 0; i < cnt_operations; ++i) {
        for (size_t j = 0; j < cnt_tools; ++j) {
            out << DurationToStr(RndDuration()) << " ";
        }
        out << std::endl;
    }

    size_t cnt_works = rand() % 5;
    for (size_t i = 0; i < cnt_works; ++i) {
        auto start = now + RndDuration();
        out << "\nwork\n"
            << TimePointToStr(start) << " "
            << TimePointToStr(start + RndDuration()) << " "
            << (double)rand() / (double)rand() << " "
            << cnt_operations / cnt_works << std::endl;
        for (size_t j = 0; j < cnt_operations / cnt_works - 1; ++j) {
            out << j + i * (cnt_operations / cnt_works) << " "
                << j + i * (cnt_operations / cnt_works) + 1 << std::endl;
        }
    }

    out.close();
}

int main() { 
    std::ifstream input("../test_data/test.txt");
    ProblemData data(input);
    return 0; 
}