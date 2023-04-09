#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <filesystem>

/* File reading */

static std::string getExploitPath(HMODULE dll)
{
    char fpath[MAX_PATH];

    if (GetModuleFileName(dll, fpath, sizeof(fpath)) == 0)
        exit(GetLastError());

    std::filesystem::path fspath = std::filesystem::path(fpath);
    std::string path = fspath.parent_path().string();

    return path;
}

static bool isfile(std::string url)
{
    std::ifstream s(url);

    bool g = s.good();
    s.close();

    return g;
}

static std::string readfile(std::string url)
{
    std::ifstream s(url);
    if (isfile(url))
    {
        std::stringstream buffer;
        buffer << s.rdbuf();

        s.close();
        return buffer.str();
    }
    return std::string();
}