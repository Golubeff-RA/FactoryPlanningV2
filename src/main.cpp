#include <iostream>
#include <fstream>
#include <random>
#include <time.h>
#include "printer.h"

using namespace std::chrono_literals;

Duration RndDuration() {
    return Duration(ch::seconds(rand()%3000));
}

int main() {
    srand(time(0));
    auto now = ch::system_clock::now();
    std::cout << "Curr time: " << TimePointToStr(now) << std::endl; 
    std::cout << "Duration = 1h : " << DurationToStr(Duration(1h));

    std::ofstream out("test.txt");
    out << "TOOLS\n";
    size_t cnt_tools = rand() % 20;
    size_t cnt_operations = rand() % 40;

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

    out << "\nOPERATIONS\n" << cnt_operations << std::endl;
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


    return 0;
}