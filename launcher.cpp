#include "processutil.hpp"
#include "taskbargroup.hpp"
#include "update.hpp"

#include <boost/json.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <fstream>
#include <exception>

#define CONFIG_FILE_NAME                    "PortableFirefox.json"
#define CONFIG_DEFAULT_VERSION              ""
#define CONFIG_DEFAULT_PARAMS               R"(-profile "..\profile")"
#define CONFIG_DEFAULT_CURL_PARAMS          R"(-k --connect-timeout 5 --proxy socks5://127.0.0.1:1080)"
#define CONFIG_DEFAULT_UPDATE_INTERVAL      72
#define CONFIG_DEFAULT_UPDATE_TIMESTAMP     0

// #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:wmainCRTStartup")

namespace pt = boost::property_tree;
namespace fs = boost::filesystem;

void write_json_without_escape_forward_slash(std::string filename, pt::ptree tree) {
    std::stringstream ss;
    pt::write_json(ss, tree);
    std::string json = ss.str();
    boost::replace_all(json, "\\/", "/");
    std::ofstream ofs(filename, std::ofstream::out | std::ofstream::trunc);
    ofs << json;
}

int wmain(int argc, wchar_t** argv) {
    std::string currentVersion;
    std::string params;
    std::string updateVersion;
    std::string curlExtraParams;
    std::wstring cmd;
    fs::wpath firefoxPath;
    fs::wpath firefoxWorkingDir;

    int updateInterval = 0;
    int currentTimestamp = 0;
    int lastCheckUpdateTimestamp = 0;

    pt::ptree root;

    // Change current working dir to exe dir.
    // Ohterwise open url from other program may cause relative path work inproperly
    fs::current_path(fs::system_complete(argv[0]).parent_path());

    try {
        // Read config
        pt::read_json(CONFIG_FILE_NAME, root);

        currentVersion = root.get<std::string>("version", CONFIG_DEFAULT_VERSION);
        params = root.get<std::string>("params", CONFIG_DEFAULT_PARAMS);
        curlExtraParams = root.get<std::string>("curl.params", "");
        updateInterval = root.get<int>("update.interval", CONFIG_DEFAULT_UPDATE_INTERVAL);
        lastCheckUpdateTimestamp = root.get<int>("update.timestamp", CONFIG_DEFAULT_UPDATE_TIMESTAMP);
    } catch (std::exception &e) {
        std::cout << "[read_json]" << e.what() << std::endl;

        root.put("version", CONFIG_DEFAULT_VERSION);
        root.put("params", CONFIG_DEFAULT_PARAMS);
        root.put("curl.[example]params", CONFIG_DEFAULT_CURL_PARAMS);
        root.put("curl.params", "-k --connect-timeout 5");
        root.put("update.interval", CONFIG_DEFAULT_UPDATE_INTERVAL);
        root.put("update.timestamp", CONFIG_DEFAULT_UPDATE_TIMESTAMP);

        // pt::write_json(CONFIG_FILE_NAME, root);
        write_json_without_escape_forward_slash(CONFIG_FILE_NAME, root);
        return 0;
    }

    if (!currentVersion.empty()) {
        // Taskbar group
        ChangeTaskBarLnkTargetPath(argv[0]);

        // Run chromium
        if (argc <= 2) {
            firefoxPath = fs::system_complete(argv[0]).parent_path() / currentVersion.c_str() / "firefox.exe";
            firefoxWorkingDir = fs::system_complete(argv[0]).parent_path() / currentVersion.c_str();

            if (argc == 2)
                cmd = boost::str(boost::wformat(L"%s %s %s") % firefoxPath % argv[1] % params.c_str());
            else
                cmd = boost::str(boost::wformat(L"%s %s") % firefoxPath % params.c_str());
            
            StartProcess(cmd.c_str(), firefoxWorkingDir.c_str());
        }
    }

    // Check update interval
    currentTimestamp = time(NULL);
    if (currentTimestamp - lastCheckUpdateTimestamp < updateInterval * 3600)
        return 0;

    // Update
    if (UpdateChromium(
            currentVersion,
            updateVersion,
            curlExtraParams) == 0)
    {
        // Get new update, so save to config
        root.put("version", updateVersion);
        root.put("update.timestamp", currentTimestamp);
        // pt::write_json(CONFIG_FILE_NAME, root);
        write_json_without_escape_forward_slash(CONFIG_FILE_NAME, root);

        // Delte old version when restart
        if (!currentVersion.empty())
            MoveFileExA(currentVersion.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
    }

    return 0;
}
