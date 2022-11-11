#include "parallel-cp.h"

#include <thread>
#include <future>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

fs::path lastComponent(const std::filesystem::path& p)
{
    return fs::relative(p, p.parent_path());
}

bool copyFile(const std::filesystem::path source, const std::filesystem::path dest)
{
    try
    {
        fs::copy_file(source, dest);
        return true;
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool parallelCopy(const std::filesystem::path source, const std::filesystem::path dest)
{
    bool res = true;
    std::vector<std::future<bool>> tasks;
    try
    {
        auto copyPath = !fs::exists(dest) ? dest : dest / lastComponent(source);
        // if (fs::exists(source))
        // {
        //     fs::create_directory(copyPath);
        // }
        fs::create_directory(copyPath, source);

        for (auto const& dir_entry : std::filesystem::directory_iterator(source))
        {
            if (dir_entry.is_regular_file())
            {
                std::future<bool> file_task = std::async(std::launch::async, copyFile, dir_entry.path(), copyPath / dir_entry.path().filename());
                tasks.push_back(std::move(file_task));
            }
            else if (dir_entry.is_directory())
            {
                std::future<bool> dir_task = std::async(std::launch::async, parallelCopy, dir_entry.path(), copyPath);
                tasks.push_back(std::move(dir_task));
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    for (auto& t : tasks)
        res &= t.get();

    return res;
}
