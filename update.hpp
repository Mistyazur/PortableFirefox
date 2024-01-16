#ifndef __UPDATE_HPP__
#define __UPDATE_HPP__

#include "processutil.hpp"

#include <boost/filesystem.hpp>

#include <regex>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define FIREFOX_LATEST_UPDATE_URL "https://download.mozilla.org/?product=firefox-latest&os=win64&lang=en-US"

namespace fs = boost::filesystem;

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
    bool needUpdate = false;
    WCHAR szCmd[MAX_PATH];
    DWORD exitcode = 1;

    // Request latest chromium info
    swprintf_s(szCmd, L"curl --head %S \"%S\"", curlExtraParams.c_str(), FIREFOX_LATEST_UPDATE_URL);
    for (int i=0; i<5 && exitcode!=0; ++i) {
        if (!StartProcessWithOutput(szCmd, NULL, res, &exitcode))
            return 1;
    }
    if (exitcode || res.empty())
        return 2;

    if (!std::regex_search(res, matches, versionRe))
        return 3;
    
    updateVersion = matches[1];

    if (currentVersion.empty()) {
        // Firefox is not installed
        needUpdate = true;
    } else {
        if (CompareVersion(updateVersion, currentVersion) == 1)
            needUpdate = true;
    }

    if (!needUpdate)
        return 4;

    // Download
    swprintf_s(szCmd, L"curl -L %S -o %S \"%S\"", curlExtraParams.c_str(), excutableName, FIREFOX_LATEST_UPDATE_URL);
    if (!StartProcess(szCmd, NULL, &exitcode))
        return 5;
    if (exitcode != 0)
        return 6;

    // Convert from self-extract exe to 7z
    if (convertExeTo7z(excutableName, archiveName) != 0)
        return 7;

    // Check zip integerity
    swprintf_s(szCmd, LR"(7za t %S >NUL 2>NUL)", archiveName);
    if (!StartProcess(szCmd, NULL, &exitcode))
        return 5;
    if (exitcode != 0) {
        fs::remove(archiveName);
        return 8;
    }
    
    // Unzip
    swprintf_s(szCmd, L"7za x %S core >NUL 2>NUL", archiveName);
    if (!StartProcess(szCmd, NULL, &exitcode)) {
        fs::remove(archiveName);
        return 9;
    }
    if (exitcode != 0) {
        fs::remove(archiveName);
        return 10;
    }

    // Delete File
    fs::remove(archiveName);

    // Rename
    fs::rename("core", updateVersion);

    // Succeeded
    return 0;
}

#endif