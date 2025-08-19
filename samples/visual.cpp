#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp> // Для работы с JSON

using json = nlohmann::json;

struct Task {
    int executorId;
    int operationId;
    std::string start; // "dd.mm.yy_hh:mm:ss"
    std::string end;
};

int main() {
    std::vector<Task> tasks = {
        {1, 101, "17.08.25_10:00:00", "17.08.25_12:00:00"},
        {1, 102, "17.08.25_13:00:00", "17.08.25_15:00:00"},
        {2, 201, "17.08.25_09:00:00", "17.08.25_11:30:00"}
    };

    // Конвертируем в JSON
    json j;
    for (const auto& task : tasks) {
        j.push_back({
            {"executor", task.executorId},
            {"operation", task.operationId},
            {"start", task.start},
            {"end", task.end}
        });
    }

    // Сохраняем в файл
    std::ofstream out("schedule.json");
    out << j.dump(4);
    out.close();

    return 0;
}