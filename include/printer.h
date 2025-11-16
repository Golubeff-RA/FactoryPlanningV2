#pragma once

#include <ostream>

#include "concurent_grid.h"
#include "defines.h"
#include "problem_data.h"

std::string TimePointToStr(TimePoint tp);

std::string DurationToStr(Duration dur);

class Printer {
public:
    static void PrintProblemData(const ProblemData& data, std::ostream& out);
    static void PrintGants(const ProblemData& data, std::ostream& out);
    static void PrintShedules(const ProblemData& data, std::ostream& out);
    static void PrintOperations(const ProblemData& data, std::ostream& out);
    static void PrintWorks(const ProblemData& data, std::ostream& out);
    static void PrintAnswerJSON(const ProblemData& data, Score score,
                                std::ostream& out);
    template <typename T>
    static void PrintGridCSV(const ConcurentGrid<T>& grid, std::ostream& out,
                             char delimiter = ';') {
        auto col_labels = grid.GetColumnLabels();
        auto row_labels = grid.GetRowLabels();
        auto [rows, cols] = grid.Size();

        out << "" << delimiter;
        for (size_t i = 0; i < col_labels.size(); ++i) {
            out << col_labels[i];
            if (i < col_labels.size() - 1) {
                out << delimiter;
            }
        }
        out << "\n";

        // Данные с названиями строк
        for (size_t i = 0; i < rows; ++i) {
            // Название строки
            std::string row_name =
                (i < row_labels.size() && !row_labels[i].empty())
                    ? row_labels[i]
                    : "Row" + std::to_string(i);
            out << row_name << delimiter;

            // Данные ячеек
            for (size_t j = 0; j < cols; ++j) {
                if (auto value = grid.Get(i, j)) {
                    out << *value;
                } else {
                    out << "";  // Пустая ячейка если недоступна
                }
                if (j < cols - 1) {
                    out << delimiter;
                }
            }
            out << "\n";
        }
    }
};