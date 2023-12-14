#ifndef __JSETTINGS_HPP__
#define __JSETTINGS_HPP__

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

#include <iostream>
#include <fstream>

using namespace rapidjson;

class JSettings {
public:
    JSettings() {};

    ~JSettings() {};

    bool open(const char *path) {
        std::ifstream ifs(path);
        if (ifs.is_open()) {
            IStreamWrapper isw {ifs};
            m_doc.ParseStream(isw);
            return true;
        }
        return false;
    };

    template <typename T>
    void set(const char *path, const T &value) {
        Pointer(path).Set(m_doc, value);
    };

    template<typename T>
    T get(const char *path) {
        return Pointer(path).Get(m_doc)->Get<T>();
    };

    bool save(const char *path) {
        std::ofstream ofs(path);
        if (ofs.is_open()) {
            OStreamWrapper osw{ofs};
            PrettyWriter<OStreamWrapper> w{osw};
            m_doc.Accept(w);
            return true;
        }
        return false;
    };


private:
    Document m_doc;
};

int testJSettings() {
    JSettings settings;
    settings.open("test.json");
    settings.set("/a/b/c.d.e", 1);
    // settings.set("/a/b/c.d.f", false);
    // settings.set("/a/b/c.d.g", "false");
    // std::cout << settings.get<int>("/a/b/c.d.e") << std::endl;;

    std::cout << settings.get<const char *>("/a/b/c.d.g") <<std::endl;
    std::cout << settings.get<std::string>("/a/b/c.d.g") <<std::endl;
    // std::cout << settings.get("/a/b/c.d.e")->GetInt() << std::endl;;
    // std::cout << settings.get("/a/b/c.d.g")->GetString() << std::endl;;
    // settings.set("/a/b/c.d.f", false);
    // settings.set("/a/b/c.d.g", "false");
    // settings.save("test.json");
    return 0;
}

#endif