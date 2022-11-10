#include "parallel-cp.h"

#include <thread>

namespace fs = std::filesystem;

fs::path lastComponent(const std::filesystem::path& p)
{
    return fs::relative(p, p.parent_path());
}

void copyFileWrapper(const std::filesystem::path source, const std::filesystem::path dest)
{
    fs::copy_file(source, dest);
}

void parallelCopy(const std::filesystem::path source, const std::filesystem::path dest)
{
    auto newPath = dest / lastComponent(source);
    fs::create_directory(newPath);
    std::vector<std::jthread> threads;
    for (auto const& dir_entry : std::filesystem::directory_iterator(source))
    {
        if (dir_entry.is_regular_file())
        {
            std::jthread file_task(copyFileWrapper, dir_entry.path(), newPath / dir_entry.path().filename());
            threads.push_back(std::move(file_task));
        }
        else if (dir_entry.is_directory())
        {
            std::jthread dir_task(parallelCopy, dir_entry.path(), newPath);
            threads.push_back(std::move(dir_task));
        }
    }

    for (auto& t : threads)
        t.join();
}
