#pragma once

#include <filesystem>

struct options {
    std::filesystem::path output_path = "gen";
    std::filesystem::path templates_folder = "templates";
    bool is_debug = false;
};
