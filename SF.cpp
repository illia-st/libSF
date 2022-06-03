#include "SF.h"

SF::SF(): root_dir{"C:\\"}, max_number_of_threads{8}, exception_file_name{"exceptions.txt"}{}

void SF::OperateDirectories(std::vector<path> dirs, const std::string &file_name) {
    //Scan all the directories that the thread has
    for(auto& dir: dirs){
        try {
            if(this->ScanDirectory(dir, file_name)) {
                break;
            }
        }
        //If we have any exceptions, we will write them to the file
        catch(std::exception& ex){
            std::scoped_lock sc(file_mutex);
            std::ofstream out(this->exception_file_name, std::ios::app);
            out << ex.what() << std::endl;
            out.close();
        }
    }
}

bool SF::ScanDirectory(path &curr_path, const std::string &file_name) {
    // check if the result is already found(it's liked the signal for other threads that the work is done)
    if (std::scoped_lock res_lock(this->res_mutex); !result.empty()){
        return true;
    }
    std::vector<path> subdirectories;
    // if not, check if we can open the current directory
    char temp_path[curr_path.string().size()];
    strcpy(temp_path, curr_path.string().c_str());
    DWORD attributes = GetFileAttributesA(temp_path);

    if (attributes & FILE_ATTRIBUTE_HIDDEN || attributes & FILE_ATTRIBUTE_SYSTEM
        || attributes & FILE_ATTRIBUTE_VIRTUAL || attributes & FILE_ATTRIBUTE_OFFLINE){
        return false;
    }
    // now analise the current directory
    for (auto &it: iterator(curr_path)) {
        if (is_directory(it.path()) && exists(it.path())) {
            subdirectories.push_back(it.path());
        } else if (it.path().filename() == file_name) {
            // return true if here is the answer
            std::scoped_lock res_lock(this->res_mutex);
            result = it.path().string();
            return true;
        }
    }

    for(auto& dirs: subdirectories){
        //if here is no answer, run ScanDirectory for the current directory's subdirectories
        if(this->ScanDirectory(dirs, file_name)){
            return true;
        }
    }
    return false;
}

bool SF::ScanRootDirectory(const std::string &file_name) {
    //Scan the root directory to find all the subdirectories
    for (auto &it: iterator(this->root_dir)) {
        if (is_directory(it.path())) {
            this->root_subdirs.push_back(it.path());
        }
            //If we find the answer here, we don't have to initialize the threads
        else if (it.path().filename() == file_name) {
            result = it.path().string();
            return true;
        }
    }
    return false;
}

std::string SF::FindPath(const std::string& file_name) {
    //clear old data which we don't need now
    this->root_subdirs.clear();
    this->result.clear();

    // create or clear file for exceptions
    std::ofstream out(this->exception_file_name);
    out.close();
    if(this->ScanRootDirectory(file_name)){
        return this->result;
    }
    //make an even distribution of data between threads
    size_t n = this->root_subdirs.size();

    size_t total_increase = n/this->max_number_of_threads;
    size_t excess = n%this->max_number_of_threads;

    std::vector<size_t> distribution(this->max_number_of_threads, total_increase);

    std::for_each_n(distribution.begin(), excess, [](auto& i){++i;});

    std::vector<std::thread> threads;

    auto first = this->root_subdirs.begin();

    //make threads
    for(size_t i {}; i < this->max_number_of_threads; ++i){
        if(distribution[i] == 0){
            break;
        }
        std::vector<path> temp;
        std::copy_n(first, distribution[i], std::back_inserter(temp));

        threads.emplace_back(&SF::OperateDirectories, std::ref(*this), std::move(temp), file_name);
        first += static_cast<int>(distribution[i]);
    }
    //make sure that every thread has finished the work
    for(auto& thr: threads){
        thr.join();
    }
    //return the result
    return this->result;
}