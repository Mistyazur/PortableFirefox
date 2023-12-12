#ifndef __TASKBARGROUP_HPP__
#define __TASKBARGROUP_HPP__

#include "appid.hpp"

#include <windows.h>
#include <shlobj.h>

#include <string>

int ChangeTaskBarLnkTargetPath(wchar_t *lpAppPath) {
    int result = 0;
    wchar_t szPath[MAX_PATH];
    std::wstring taskbarPath;

    APPID::initialize();

    try {
        if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
            throw -1;

        taskbarPath = szPath;
        taskbarPath.append(TEXT("\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar\\Chromium.lnk"));

        if (!APPID::getLnkAppPath(taskbarPath.c_str(), szPath, MAX_PATH))
            throw -2;

        if (wcsicmp(szPath, lpAppPath) != 0) {
            wcscpy_s(szPath, lpAppPath);
            if (!APPID::setLnkAppPath(taskbarPath.c_str(), szPath))
                throw -3;
        }
    } catch (int e) {
        result = e;
    }

    APPID::uninitialize();

    return result;
}

#endif