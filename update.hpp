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

#define UNGOOGLED_CHROMIUM_LATEST_UPDATE_URL "https://api.github.com/repos/%s/releases/latest"

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

int UpdateChromium(
    const std::string &currentVersion,
    std::string &updateVersion,
    const std::string &curlExtraParams,
    const std::string &githubRepo,
    const std::string &githubAsset,
    const std::string &githubMirror
)
{
    std::string res, cmd, downloadUrl;
    std::regex re(githubAsset);
    pt::ptree root, assets;
    boost::format format;
    int exitcode = -1;
    bool needUpdate = false;

    // Request latest chromium info
    format = boost::format("curl %s %s") % curlExtraParams % UNGOOGLED_CHROMIUM_LATEST_UPDATE_URL;
    format = boost::format(format.str()) % githubRepo;
    cmd = format.str();
    std::cout << "curl: " << cmd << std::endl;
    for (int i=0; i<5 && exitcode!=0; ++i) {
        std::cout << "Try get latest update info: ";
        res = ExecCmd(cmd.c_str(), &exitcode);
        std::cout << exitcode << std::endl;
    }
    if (exitcode || res.empty())
        return 1;

    //
    try {
        std::stringstream ss;
        ss << res;
        pt::read_json(ss, root);
        updateVersion = root.get<std::string>("name", "");
        std::cout << "New Version: " << updateVersion << std::endl;
    } catch (...) {
        return 2;
    }

    if (currentVersion.empty()) {
        // Chromium is not installed
        needUpdate = true;
    } else {
        if (CompareVersion(updateVersion, currentVersion) == 1)
            needUpdate = true;
    }

    if (!needUpdate)
        return 3;
    
    // Get download url
    assets = root.get_child("assets");
    for (auto &asset : assets) {
        res = asset.second.get<std::string>("name", "");
        if (std::regex_match(res, re)) {
            downloadUrl = asset.second.get<std::string>("browser_download_url", "");
            break;
        }
    }
    if (downloadUrl.empty())
        return 4;

    // Optimize github download url
    boost::replace_first(downloadUrl, "https://github.com", githubMirror);

    // Download
    cmd = boost::str(boost::format(
        "curl -L -o ungoogled-chromium.zip %s %s") % curlExtraParams % downloadUrl);
    std::cout << "curl: " << cmd << std::endl;
    ExecCmd(cmd.c_str(), &exitcode);
    if (exitcode != 0)
        return 5;

    return 11;
    // Check zip integerity
    ExecCmd(R"(7za t ungoogled-chromium.zip >NUL 2>NUL)", &exitcode);
    if (exitcode != 0) {
        DeleteFileW(L"ungoogled-chromium.zip");
        return 6;
    }
    
    // Unzip
    cmd = boost::str(boost::format(
        "7za x ungoogled-chromium.zip >NUL 2>NUL && move ungoogled-chromium_* %s") % updateVersion);
    res = ExecCmd(cmd.c_str(), &exitcode);
    if (exitcode != 0) {
        DeleteFileW(L"ungoogled-chromium.zip");
        return 7;
    }

    // Delete File
    DeleteFileW(L"ungoogled-chromium.zip");

    // Succeeded
    return 0;
}

#endif