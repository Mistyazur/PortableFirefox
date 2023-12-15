#include "processutil.hpp"
#include "taskbargroup.hpp"
#include "update.hpp"
#include "jsettings.hpp"

#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>
#include <vector>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:wmainCRTStartup")

#define FIREFOX_DIR                         "firefox"
#define OBSOLETE_DIR                        "obsolete"

#define CONFIG_FILE_NAME                    "PortableFirefox.json"


namespace fs = boost::filesystem;

bool ParseFirefoxAutoConfig(JSettings &js) {
    std::string &autoconfig = js.getObjStr("/autoconfig/config", "");
    std::string autoconfigFunc(js.get<const char *>("/autoconfig/func", ""));
    if (autoconfig.empty() || autoconfigFunc.empty())
        return true;

    if (autoconfig.at(0) != '{' && autoconfig.at(autoconfig.size() - 1) != '}')
        return false;
    autoconfig = std::string(autoconfig.begin() + 1, autoconfig.end() - 1);

    std::ostringstream oss;
    std::vector<std::string> vec;
    boost::split(vec, autoconfig, boost::is_any_of(","));
    oss << "// IMPORTANT: Start your code on the 2nd line" << std::endl;
    for (auto &v : vec) {
        size_t pos = v.find_first_of(":");
        if (pos == std::string::npos)
            return false;

        boost::format format = boost::format("%s(%s, %s);") % autoconfigFunc % v.substr(0, pos) % v.substr(pos + 1, v.size() - pos - 1);
        oss << format.str() << std::endl;
    }

    fs::path autoconfigJS = fs::current_path() / FIREFOX_DIR / "\\defaults\\pref\\autoconfig.js";
    std::ofstream ofsJS(autoconfigJS.c_str());
    if (!ofsJS.is_open())
        return false;
    ofsJS <<
R"(// IMPORTANT: Start your code on the 2nd line
pref("general.config.obscure_value", 0);
pref("general.config.filename", "autoconfig.cfg");)";

    fs::path path = fs::current_path() / FIREFOX_DIR / "autoconfig.cfg";
    std::ofstream ofsCFG(path.c_str());
    if (!ofsCFG.is_open())
        return false;
    ofsCFG << oss.str();

    return true;
}

int wmain(int argc, wchar_t** argv) {
    JSettings js;
    std::string currentVersion;
    std::string params;
    std::string updateVersion;
    std::string curlExtraParams;
    std::wstring cmd;
    boost::format format;
    fs::wpath firefoxPath;
    fs::wpath firefoxWorkingDir;

    int updateInterval = 0;
    int currentTimestamp = time(NULL);
    int lastCheckUpdateTimestamp = 0;
    int err = 0;

    // Change current working dir to exe dir.
    // Ohterwise open url from other program may cause relative path work inproperly
    fs::current_path(fs::system_complete(argv[0]).parent_path());

    // Create default config if not exists
    if (!fs::exists(CONFIG_FILE_NAME)) {
        std::ofstream ofs(CONFIG_FILE_NAME);
        if (!ofs.is_open())
            return 1;
        ofs <<
R"({
  "version": "",
  "params": "-profile \"..\\profile\"",
  "curl": {
    "params": "-k --connect-timeout 10"
  },
  "update": {
    "interval": 72,
    "timestamp": 0
  },
  "autoconfig": {
    "func": "lockPref",
    "config": {
      "extensions.pocket.enabled": false,
      "browser.cache.disk.capacity": 1048576,
      "browser.newtabpage.activity-stream.feeds.telemetry": false,
      "browser.newtabpage.activity-stream.telemetry": false,
      "browser.ping-centre.telemetry": false,
      "datareporting.healthreport.service.enabled": false,
      "datareporting.healthreport.uploadEnabled": false,
      "datareporting.policy.dataSubmissionEnabled": false,
      "datareporting.sessions.current.clean": true,
      "devtools.onboarding.telemetry.logged": false,
      "toolkit.telemetry.archive.enabled": false,
      "toolkit.telemetry.bhrPing.enabled": false,
      "toolkit.telemetry.enabled": false,
      "toolkit.telemetry.firstShutdownPing.enabled": false,
      "toolkit.telemetry.hybridContent.enabled": false,
      "toolkit.telemetry.newProfilePing.enabled": false,
      "toolkit.telemetry.prompted": 2,
      "toolkit.telemetry.rejected": true,
      "toolkit.telemetry.reportingpolicy.firstRun": false,
      "toolkit.telemetry.server": "",
      "toolkit.telemetry.shutdownPingSender.enabled": false,
      "toolkit.telemetry.unified": false,
      "toolkit.telemetry.unifiedIsOptIn": false,
      "toolkit.telemetry.updatePing.enabled": false
    }
  }
})";
    }

    // Read config
    if (!js.load(CONFIG_FILE_NAME))
        return 2;

    currentVersion = js.get<const char *>("/version", "");
    params = js.get<const char *>("/params", R"(-profile "..\profile")");
    curlExtraParams = js.get<const char *>("/curl/params", "-k --connect-timeout 5");
    updateInterval = js.get<int>("/update/interval", 72);
    lastCheckUpdateTimestamp = js.get<int>("/update/timestamp", 0);

    // Apply update
    try {
        if (fs::exists(OBSOLETE_DIR))
            fs::remove_all(OBSOLETE_DIR);

        if (fs::exists(currentVersion)) {
            // Need rename current version dir to firefox dir
            if (!fs::exists(FIREFOX_DIR)) {
                fs::rename(currentVersion, FIREFOX_DIR);
            } else {
                fs::rename(FIREFOX_DIR, OBSOLETE_DIR);
                try {
                    fs::rename(currentVersion, FIREFOX_DIR);
                } catch (fs::filesystem_error e) {
                    fs::rename(OBSOLETE_DIR, FIREFOX_DIR);
                }
            }
        }
    } catch (fs::filesystem_error e) {
        std::cout << e.what() << std::endl;
    }

    // Start firefox
    if (fs::exists(FIREFOX_DIR)) {
        // Taskbar group
        ChangeTaskBarLnkTargetPath(argv[0]);

        // Firefox autoconfig
        if (!ParseFirefoxAutoConfig(js))
            return 3;

        // Run chromium
        if (argc <= 2) {
            firefoxPath = fs::system_complete(argv[0]).parent_path() / FIREFOX_DIR / "firefox.exe";
            firefoxWorkingDir = fs::system_complete(argv[0]).parent_path() / FIREFOX_DIR;

            if (argc == 2)
                cmd = boost::str(boost::wformat(L"%s %s %s") % firefoxPath % argv[1] % params.c_str());
            else
                cmd = boost::str(boost::wformat(L"%s %s") % firefoxPath % params.c_str());
            
            StartProcess(cmd.c_str(), firefoxWorkingDir.c_str());
        }

        // Check update interval
        if (currentTimestamp - lastCheckUpdateTimestamp < updateInterval * 3600)
            return 0;
    } else {
        currentVersion.clear();
    }

    // Update
    js.set("/update/timestamp", currentTimestamp);
    if (UpdateChromium(currentVersion, updateVersion, curlExtraParams) == 0) {
        // Update succeeded
        js.set("/version", updateVersion.c_str());
    }

    return 0;
}
