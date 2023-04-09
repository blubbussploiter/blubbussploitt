#include "content.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <filesystem>

#include "files.h"

/* Checks */

bool RBX::ContentId::isRbxAssetId()
{
    return parsed.scheme_ == "rbxassetid";
}

bool RBX::ContentId::isRbxLocalAsset()
{
    return parsed.scheme_ == "rbxasset";
}

/* Returning strings */

std::string RBX::ContentId::findLocalPath()
{
    try
    {
        std::string basePath = parsed.host_; /* rbxasset://[fonts]/character.rbxm */
        std::string path = parsed.path_; /* rbxasset://fonts/[character.rbxm] */

        /* Combine host and path since the parser intakes ://____/ as the host and /___/.* as the path */

        return basePath + "\\" + path;
    }
    catch (std::exception)
    {
        return std::string();
    }
}

/* Local assets */

std::string RBX::ContentId::findContentPath()
{
    std::filesystem::path content = std::filesystem::current_path() / "content";
    return content.string();
}

std::string RBX::ContentId::findContentByPath(std::string path)
{
    std::string cpath = findContentPath();
    std::string fpath = cpath + "\\" + path;

    if (isfile(fpath))
        return readfile(fpath);

    return std::string();
}

/* Web assets */

std::string RBX::ContentId::resolveById()
{
    std::string url = std::format("https://crapblox.cf/asset?id={}", id);
    return DownloadURL(url.c_str());
}

int RBX::ContentId::findId()
{
    try
    {
        std::string path = parsed.host_;
        return std::stoi(path);
    }
    catch (std::exception)
    {
        return 0;
    }
}

/* Final call */

std::string RBX::ContentId::resolve()
{
    if (isRbxAssetId())
    {
        id = findId();
        return resolveById();
    }
    if (isRbxLocalAsset())
    {
        std::string path = findLocalPath();
        return findContentByPath(path);
    }

    /* If neither, check if it is a full on url (not rbxassetid) or if its a file on the machine */

    if (isfile(url))
        return readfile(url);

    std::string downloaded = DownloadURL(url.c_str());
    if (downloaded != "")
        return downloaded;

    return "";
}