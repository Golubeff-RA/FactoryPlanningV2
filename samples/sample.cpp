#include <time.h>

#include <fstream>
#include <iostream>
#include <random>

#include "generator.h"
#include "printer.h"
#include "problem_data.h"
#include "solver.h"

using namespace std::chrono_literals;

Duration RndDuration() { return Duration(ch::seconds(rand() % 3000)); }

int main() {
    Generator gen;
    ProblemData data(gen.Generate(5, 5, ch::system_clock::now(), 100));
    Printer::PrintProblemData(data, std::cout);
}