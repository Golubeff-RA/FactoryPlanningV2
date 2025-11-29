#pragma once

#include <atomic>
#include <cstddef>
#include <string>

#include "basics/problem_data.h"

class Dataset {
public:
    static int CreateDataset(const std::string& path_name, size_t size);

    Dataset(const std::string& folder_path,
            std::vector<std::string>& file_names);

    std::optional<std::string> GetNext(ProblemData* data);

private:
    std::vector<std::string> file_paths_;
    std::atomic<size_t> current_index_{0};
};