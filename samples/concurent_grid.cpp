#include "utils/concurent_grid.h"

#include <iostream>
#include <thread>

#include "utils/printer.h"

int main() {
    ConcurentGrid<int> grid(3, 4, {"Alice", "Bob", "Charlie"},
                            {"Jan", "Feb", "Mar", "Apr"}, 6);
    // Запись по индексам
    grid.Set(0, 1, 100);
    grid.Set(1, 2, 200);

    // Запись по меткам
    grid.Set("Alice", "Mar", 150);
    grid.Set("Bob", "Jan", 250);

    // Многопоточное использование
    std::vector<std::thread> threads;

    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&grid, i]() {
            // Каждый поток записывает в случайные ячейки
            for (int j = 0; j < 100; ++j) {
                size_t row = rand() % 3;
                size_t col = rand() % 4;
                grid.Set(row, col, i * 10 + j);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Чтение данных
    if (auto value = grid.Get("Alice", "Feb")) {
        std::cout << "Alice/Feb: " << *value << std::endl;
    }

    Printer::PrintGridCSV(grid, std::cout);
    return 0;
}