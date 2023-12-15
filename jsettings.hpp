#ifndef __JSETTINGS_HPP__
#define __JSETTINGS_HPP__

#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

#include <iostream>
#include <fstream>

using namespace rapidjson;

class JSettings {
public:
    JSettings() {};

    ~JSettings() {
        save();
    };

    bool load(const char *path) {
        if (strlen(path) == 0)
            return false;

        m_path = path;
        std::fstream ifs(path);
        if (ifs.is_open()) {
            IStreamWrapper isw {ifs};
            m_doc.ParseStream(isw);
            return true;
        }
        return false;
    };

    bool save(const char *path = nullptr) {
        if (path == nullptr)
            path = m_path.c_str();

        if (m_doc.Empty() || strlen(path) == 0)
            return false;

        std::ofstream ofs(path);
        if (ofs.is_open()) {
            OStreamWrapper osw{ofs};
            PrettyWriter<OStreamWrapper> w{osw};
            m_doc.Accept(w);
            return true;
        }
        return false;
    };


    template <typename T>
    void set(const char *path, const T &value) {
        Pointer(path).Set(m_doc, value);
    };

    // // This will create path with default value
    // template<typename T>
    // T get(const char *path, const T default) {
    //     return Pointer(path).GetWithDefault(m_doc, default).Get<T>();
    // };

    // This will not create path with default value
    template<typename T>
    T get(const char *path, const T default) {
        Value *val = Pointer(path).Get(m_doc);
        if (!val)
            return default;
        return val->Get<T>();
    };

    std::string getObjStr(const char *path, const char *default) {
        Value *val = Pointer(path).Get(m_doc);
        if (!val || !val->IsObject())
            return default;

        StringBuffer strbuf;
        Writer<StringBuffer> writer(strbuf);
        val->Accept(writer);
        std::cout << strbuf.GetString() << std::endl;
        return strbuf.GetString();
    }

private:
    Document m_doc;
    std::string m_path;
};

int testJSettings() {
    JSettings settings;
    settings.load("PortableFirefox.json");
    std::cout << settings.getObjStr("/autoconfig", "123") <<std::endl;
    // std::cout << settings.get<int>("/a/b/c.d.e", 1) << std::endl;
    settings.save();
    return 0;
}

#endif