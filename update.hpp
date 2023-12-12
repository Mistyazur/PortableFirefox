#ifndef __UPDATE_HPP__
#define __UPDATE_HPP__

#include "processutil.hpp"

#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <regex>
#include <string>
#include <iostream>

#define FIREFOX_LATEST_UPDATE_URL "https://download.mozilla.org/?product=firefox-latest&os=win64&lang=en-US"

namespace pt = boost::property_tree;

int CompareVersion(const std::string& a, const std::string& b)
{
    // First, split the string.
    std::regex re("[.-]");
    std::sregex_token_iterator af{a.begin(), a.end(), re, -1}, al;//the '-1' is what makes the regex split (-1 := what was not matched)
    std::vector<std::string> va{af, al};
    std::sregex_token_iterator bf{b.begin(), b.end(), re, -1}, bl;//the '-1' is what makes the regex split (-1 := what was not matched)
    std::vector<std::string> vb{bf, bl};

    // Compare the numbers step by step, but only as deep as the version
    // with the least elements allows.
    const int depth = min(va.size(), vb.size());
    int ia, ib;
    for (int i=0; i<depth; ++i) {
        ia = atoi(va[i].c_str());
        ib = atoi(vb[i].c_str());
        if (ia != ib)
            break;
    }

    // Return the required number.
    if (ia > ib)
        return 1;
    else if (ia < ib)
        return -1;
    else {
        // In case of equal versions, assumes that the version
        // with the most elements is the highest version.
        if (va.size() > vb.size())
            return 1;
        else if (va.size() < vb.size())
            return -1;
    }

    // Everything is equal, return 0.
    return 0;
}

int convertExeTo7z(const char *fileExe, const char *file7z)
{
    std::ifstream ifs(fileExe, std::ios::binary);
    if(!ifs)
        return 1;

    std::vector<char> data = std::vector<char>(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    // const std::vector<char> pattern{'\r', '\n', '\r', '\n'};
    const char *pattern = "!@InstallEnd@!";
    auto it = std::search(data.begin(), data.end(), pattern, pattern + strlen(pattern));

    std::ofstream ofs(file7z, std::ios::binary);
    if(!ofs)
        return 2;

    std::copy(it + strlen(pattern), data.end(), std::ostreambuf_iterator<char>(ofs));

    ifs.close();
    ofs.close();

    DeleteFileA(fileExe);

    return 0;
}

int UpdateChromium(
    const std::string &currentVersion,
    std::string &updateVersion,
    const std::string &curlExtraParams
)
{
    const char *excutableName = "firefox.exe";
    const char *archiveName = "firefox.7z";
    std::string res, downloadUrl;
    std::regex versionRe(R"(/firefox/releases/([\d+\.]+)/)");
    std::smatch matches;
    pt::ptree root, assets;
    boost::format format;
    int exitcode = -1;
    bool needUpdate = false;

    // Request latest chromium info
    format = boost::format("curl --head %s \"%s\"") % curlExtraParams % FIREFOX_LATEST_UPDATE_URL;
    std::cout << "curl: " << format.str() << std::endl;
    for (int i=0; i<5 && exitcode!=0; ++i) {
        std::cout << "Try get latest update info: ";
        res = ExecCmd(format.str().c_str(), &exitcode);
        std::cout << exitcode << std::endl;
    }
    if (exitcode || res.empty())
        return 1;

    if (!std::regex_search(res, matches, versionRe))
        return 2;
    
    updateVersion = matches[1];

    if (currentVersion.empty()) {
        // Firefox is not installed
        needUpdate = true;
    } else {
        if (CompareVersion(updateVersion, currentVersion) == 1)
            needUpdate = true;
    }

    if (!needUpdate)
        return 3;

    // Download
    format = boost::format("curl -L %s -o %s \"%s\"") % curlExtraParams % excutableName % FIREFOX_LATEST_UPDATE_URL;
    std::cout << "curl: " << format.str() << std::endl;
    ExecCmd(format.str().c_str(), &exitcode);
    if (exitcode != 0)
        return 4;

    // Convert from self-extract exe to 7z
    if (convertExeTo7z(excutableName, archiveName) != 0)
        return 5;

    // Check zip integerity
    format = boost::format(R"(7za t %s >NUL 2>NUL)")  % archiveName;
    ExecCmd(format.str().c_str(), &exitcode);
    std::cout << "7za: " << format.str() << std::endl;
    if (exitcode != 0) {
        DeleteFileA(archiveName);
        return 6;
    }
    
    // Unzip
    format = boost::format(
                 "7za x %s core >NUL 2>NUL && move core %s") % archiveName % updateVersion;
    std::cout << "7za: " << format.str() << std::endl;
    res = ExecCmd(format.str().c_str(), &exitcode);
    if (exitcode != 0) {
        DeleteFileA(archiveName);
        return 7;
    }

    // Delete File
    DeleteFileA(archiveName);

    // Succeeded
    return 0;
}

#endif