#include "processutil.hpp"
#include "taskbargroup.hpp"
#include "update.hpp"
#include "jsettings.hpp"

#include <boost/json.hpp>
#include <boost/filesystem.hpp>
#include <boost/exception/exception.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <fstream>
#include <exception>

#define FIREFOX_DIR                         "firefox"
#define OBSOLETE_DIR                        "obsolete"

#define CONFIG_FILE_NAME                    "PortableFirefox.json"
#define CONFIG_DEFAULT_VERSION              ""
#define CONFIG_DEFAULT_PARAMS               R"(-profile "..\profile")"
#define CONFIG_DEFAULT_CURL_PARAMS          R"(-k --connect-timeout 5 --proxy socks5://127.0.0.1:1080)"
#define CONFIG_DEFAULT_UPDATE_INTERVAL      72
#define CONFIG_DEFAULT_UPDATE_TIMESTAMP     0

#define AUTO_CONFIG_JS                      "pref(\"general.config.obscure_value\", 0);\npref(\"general.config.filename\", \"autoconfig.cfg\");\n"

// #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:wmainCRTStartup")

namespace pt = boost::property_tree;
namespace fs = boost::filesystem;
namespace json = boost::json;

void write_json_without_escape_forward_slash(std::string filename, pt::ptree tree) {
    std::stringstream ss;
    pt::write_json(ss, tree);
    std::string json = ss.str();
    boost::replace_all(json, "\\/", "/");
    std::ofstream ofs(filename, std::ofstream::out | std::ofstream::trunc);
    ofs << json;
}

void ParseFirefoxAutoConfig() {
    std::ifstream ifs(CONFIG_FILE_NAME);
    std::string jsonStr((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    boost::json::object jsonObj = boost::json::parse(jsonStr).as_object();

    auto jsonValAutoconfig = jsonObj.if_contains("autoconfig");
    if (jsonValAutoconfig) {
        fs::path autoconfigJS = fs::current_path() / FIREFOX_DIR / "\\defaults\\pref\\autoconfig.js";
        std::ofstream ofsJS(autoconfigJS.c_str());
        ofsJS << AUTO_CONFIG_JS;

        fs::path path = fs::current_path() / FIREFOX_DIR / "autoconfig.cfg";
        std::ofstream ofs(path.c_str());
        ofs << "//" << std::endl;
        for (auto &i : jsonValAutoconfig->as_object()) {
            boost::format format = boost::format("pref(\"%s\", %s)") % i.key() % i.value();
            ofs << format << std::endl;
        }
    }
}

std::string prettifyJson(const std::string& jsonString) {
    std::stringstream ss;
    int indentLevel = 0;
    bool inQuote = false;
    char prevChar = '\0';

    for (char const& currChar : jsonString) {
        switch (currChar) {
            case '"':
                if (prevChar != '\\') {
                    inQuote = !inQuote;
                }
                ss << currChar;
                break;
            case '{':
            case '[':
                ss << currChar;
                if (!inQuote) {
                    ss << '\n';
                    indentLevel++;
                    for (int i = 0; i < indentLevel; i++) {
                        ss << '\t';
                    }
                }
                break;
            case '}':
            case ']':
                if (!inQuote) {
                    ss << '\n';
                    indentLevel--;
                    for (int i = 0; i < indentLevel; i++) {
                        ss << '\t';
                    }
                }
                ss << currChar;
                break;
            case ',':
                ss << currChar;
                if (!inQuote) {
                    ss << '\n';
                    for (int i = 0; i < indentLevel; i++) {
                        ss << '\t';
                    }
                }
                break;
            case ':':
                ss << currChar;
                if (!inQuote) {
                    ss << ' ';
                }
                break;
            default:
                ss << currChar;
        }
        prevChar = currChar;
    }

    return ss.str();
}

json::value fetchValueByPath(const json::object& obj, const std::string& path) {
    json::object curr = obj;

    std::istringstream iss(path);
    std::string segment;
    while (std::getline(iss, segment, '/')) {
        if (segment.empty()) {
            continue;
        }
        std::cout << segment << std::endl;

        auto it = curr.find(segment);
        if (it == curr.end()) {
            return json::value();
        }

        if (it->value().is_object()) {
            curr = it->value().as_object();
        } else {
            if (boost::ends_with(path, segment))
                return it->value();
            else
                return json::value();
        }
    }

    return json::value();
}

template<typename T> 
bool setValueByPath(json::object& obj, const std::string& path, const T& value) {
    json::object curr = obj;

    std::istringstream iss(path);
    std::string segment;

    while (std::getline(iss, segment, '/')) {
        if (segment.empty()) {
            continue;
        }

        auto ptr = curr.if_contains(segment);
        if (ptr) {
            if (ptr->is_object()) {
                curr = ptr->as_object();
            } else {
                if (!boost::ends_with(path, segment))
                    return false;

                *ptr = value;
                return true;
            }
        } else {
            if (boost::ends_with(path, segment)) {
                curr.emplace(segment, value);
                return true;
            }

            curr.emplace(segment, json::object());
            curr = curr.at(segment).as_object();
        }
    }


    return false;
}

void test() {
    // std::string t = R"({A:"B",C:{D:"E",F:{G:"H",I:"J"}}})";
    // std::cout << prettifyJson(t) << std::endl;

    // root.put("version", CONFIG_DEFAULT_VERSION);
    // root.put("params", CONFIG_DEFAULT_PARAMS);
    // root.put("curl.[example]params", CONFIG_DEFAULT_CURL_PARAMS);
    // root.put("curl.params", "-k --connect-timeout 5");
    // root.put("update.interval", CONFIG_DEFAULT_UPDATE_INTERVAL);
    // root.put("update.timestamp", CONFIG_DEFAULT_UPDATE_TIMESTAMP);
    
    // js::object root, child;
    // root.emplace("num", 2);
    // root.emplace("b", false);
    // root.emplace("s", "false");
    // child = root;
    // root.emplace("c", child);
    // std::cout << js::serialize(root) << std::endl;

    json::object obj {
        {"version", CONFIG_DEFAULT_VERSION},
        {"params", CONFIG_DEFAULT_PARAMS},
        {"curl", {
            {"params", CONFIG_DEFAULT_CURL_PARAMS}
        }},
        {"update", {
            {"interval", CONFIG_DEFAULT_UPDATE_INTERVAL},
            {"timestamp", CONFIG_DEFAULT_UPDATE_TIMESTAMP}
        }},
        {"autoconfig", {
            {"extensions.pocket.enabled", false},
            {"browser.cache.disk.capacity", 1048576},
            // Disable telemetry
            {"browser.newtabpage.activity-stream.feeds.telemetry", false},
            {"browser.newtabpage.activity-stream.telemetry", false},
            {"browser.ping-centre.telemetry", false},
            {"datareporting.healthreport.service.enabled", false},
            {"datareporting.healthreport.uploadEnabled", false},
            {"datareporting.policy.dataSubmissionEnabled", false},
            {"datareporting.sessions.current.clean", true},
            {"devtools.onboarding.telemetry.logged", false},
            {"toolkit.telemetry.archive.enabled", false},
            {"toolkit.telemetry.bhrPing.enabled", false},
            {"toolkit.telemetry.enabled", false},
            {"toolkit.telemetry.firstShutdownPing.enabled", false},
            {"toolkit.telemetry.hybridContent.enabled", false},
            {"toolkit.telemetry.newProfilePing.enabled", false},
            {"toolkit.telemetry.prompted", 2},
            {"toolkit.telemetry.rejected", true},
            {"toolkit.telemetry.reportingpolicy.firstRun", false},
            {"toolkit.telemetry.server", ""},
            {"toolkit.telemetry.shutdownPingSender.enabled", false},
            {"toolkit.telemetry.unified", false},
            {"toolkit.telemetry.unifiedIsOptIn", false},
            {"toolkit.telemetry.updatePing.enabled", false}
        }}
    };
    // std::cout << json::serialize(obj) << std::endl;
    // std::cout << prettifyJson(json::serialize(obj)) << std::endl;
    try {
        // std::cout << obj.find("update") << std::endl;
        // if (obj.find("update")->value().as_object().find("interval") == obj.end())
        //     std::cout  << 1 << std::endl;
        // std::cout << obj.find("update.interval") << std::endl;
        // std::cout << obj.find("/update/interval") << std::endl;

        // std::cout << fetchValueByPath(obj, "/update/interval") << std::endl;
        // std::cout << prettifyJson(json::serialize(obj)) << std::endl;

        // auto it = obj.if_contains("curl");
        // if (it)
        //     std::cout << it->as_object() << std::endl;
        std::cout << setValueByPath<int>(obj, "/curl/pb", 456) << std::endl;
        std::cout << prettifyJson(json::serialize(obj)) << std::endl;

    } catch (std::exception &e) {
        std::cout  << e.what() << std::endl;
    }

}

using namespace boost::json;

template<typename T> 
void setChildObjectByPath(object &obj, const std::string &path, const T &childObj) {
    std::vector<std::string> keys;
    std::size_t startPos = 1; // Start from index 1 to skip the leading '/'
    std::size_t endPos = path.find('/', startPos);

    while (endPos != std::string::npos) {
        std::string key = path.substr(startPos, endPos - startPos);
        keys.push_back(key);

        startPos = endPos + 1;
        endPos = path.find('/', startPos);
    }

    // The last part of the path
    std::string lastKey = path.substr(startPos);
    keys.push_back(lastKey);

    object* currentObj = &obj;

    for (const std::string& key : keys) {
        // Get or create the child object
        currentObj = &(currentObj->emplace(key, object{}).first->value().get_object());
    }

    // Set the final child object
    *currentObj = std::move(childObj);
}

void test2() {
    object parent;
    object child_of_child;

    child_of_child["foo"] = 123;
    child_of_child["bar"] = true;

    setChildObjectByPath<int>(parent, "/parent/child/child_of_child", 123);

    std::cout << parent << std::endl;
}

int wmain(int argc, wchar_t** argv) {
    testtest();
    return 0;
    std::string currentVersion;
    std::string params;
    std::string updateVersion;
    std::string curlExtraParams;
    std::wstring cmd;
    boost::format format;
    fs::wpath firefoxPath;
    fs::wpath firefoxWorkingDir;

    int updateInterval = 0;
    int currentTimestamp = 0;
    int lastCheckUpdateTimestamp = 0;
    int err = 0;

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

        // Firefox auto config
        ParseFirefoxAutoConfig();

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
        currentTimestamp = time(NULL);
        if (currentTimestamp - lastCheckUpdateTimestamp < updateInterval * 3600)
            return 0;
    } else {
        currentVersion.clear();
    }

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
    }

    return 0;
}
