#pragma once
#include <string>
#include <filesystem>
#include <thread>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <mutex>

class SF{
private:
    using iterator = std::filesystem::directory_iterator;
    using path = std::filesystem::path;
    using const_path = const std::filesystem::path;

    std::mutex res_mutex;
    std::mutex file_mutex;
    std::string result;

    bool ScanDirectory(path &curr_path, const std::string &file_name);
    void OperateDirectories(std::vector<path> dirs, const std::string &file_name);
    bool ScanRootDirectory(const std::string &file_name);

    std::vector<path> root_subdirs;
    path root_dir;
    size_t max_number_of_threads;

    std::string exception_file_name;
public:
    SF();
    std::string FindPath(const std::string& file_name);
};