@echo off
mode con cols=35 lines=15  
color 2F 
@ECHO OFF&PUSHD %~DP0
@%1 %2
@mshta vbscript:createobject("shell.application").shellexecute("%~s0","goto :runas","","runas",1)(window.close)&goto :eof
@:runas


set "PORTABLE_FIREFOX_NAME=PortableFirefox"
set "PORTABLE_FIREFOX_NAME_SHORT=PortableFirefox"
set "PORTABLE_FIREFOX_DESCRIPTION=Firefox portable launcher and updater"
set "PORTABLE_FIREFOX_PATH=%~dp0PortableFirefox.exe"
set "PORTABLE_FIREFOX_ICON=\"%PORTABLE_FIREFOX_PATH%\",0"
set "PORTABLE_FIREFOX_ARGS=\"%PORTABLE_FIREFOX_PATH%\" \"%%1\""


echo.
echo =============FIREFOX=============
echo.
echo.
echo 1. Set default browser
echo.
echo.
echo 2. Unset default browser 
echo.
echo.
echo.
set /p  ID=Input No.:
if "%id%"=="1" GOTO SetDefaultBrowser

if "%id%"=="2" GOTO UnsetDefaultBrowser

if "%id%"=="3"  goto EnableAutoConfig

if "%id%"=="4"  goto DisableAutoConfig


:SetDefaultBrowser

if not exist "%PORTABLE_FIREFOX_PATH%" (
	echo ERROR: "%PORTABLE_FIREFOX_PATH%" not found.
    PAUSE
)

reg add "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%HTML" /v "" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME% Document" /f
reg add "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%HTML\DefaultIcon" /v "" /t REG_SZ /d "%PORTABLE_FIREFOX_ICON%" /f
reg add "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%HTML\shell\open\command" /v "" /t REG_SZ /d "%PORTABLE_FIREFOX_ARGS%" /f

reg add "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%URL" /v "" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME% Protocol" /f
reg add "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%URL" /v "EditFlags" /t REG_DWORD /d "2" /f
reg add "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%URL" /v "FriendlyTypeName" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME% Protocol" /f
reg add "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%URL" /v "URL Protocol" /t REG_SZ /d "" /f
reg add "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%URL\DefaultIcon" /v "" /t REG_SZ /d "%PORTABLE_FIREFOX_ICON%" /f
reg add "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%URL\shell\open\command" /v "" /t REG_SZ /d "%PORTABLE_FIREFOX_ARGS%" /f

reg add "HKLM\Software\RegisteredApplications" /v "%PORTABLE_FIREFOX_NAME_SHORT%" /t REG_SZ /d "Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities" /f

reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%" /v "" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME%" /f

reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\DefaultIcon" /v "" /t REG_SZ /d "%PORTABLE_FIREFOX_ICON%" /f

reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\shell\open\command" /v "" /t REG_SZ /d "\"%PORTABLE_FIREFOX_PATH%\"" /f

reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\InstallInfo" /v "IconsVisible" /t REG_DWORD /d "1" /f

reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities" /v "ApplicationIcon" /t REG_SZ /d "%PORTABLE_FIREFOX_ICON%" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities" /v "ApplicationName" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME%" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities" /v "ApplicationDescription" /t REG_SZ /d "%PORTABLE_FIREFOX_DESCRIPTION%" /f

reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\FileAssociations" /v ".htm" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%HTML" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\FileAssociations" /v ".html" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%HTML" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\FileAssociations" /v ".pdf" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%HTML" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\FileAssociations" /v ".shtml" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%HTML" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\FileAssociations" /v ".svg" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%HTML" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\FileAssociations" /v ".webp" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%HTML" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\FileAssociations" /v ".xht" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%HTML" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\FileAssociations" /v ".xhtml" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%HTML" /f

reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\StartMenu" /v "StartMenuInternet" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%" /f

reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "ftp" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "http" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "https" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "mailto" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "webcal" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "urn" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "tel" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "smsto" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "sms" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "nntp" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "news" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "mms" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f
reg add "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%\Capabilities\URLAssociations" /v "irc" /t REG_SZ /d "%PORTABLE_FIREFOX_NAME_SHORT%URL" /f

%windir%\system32\control.exe /name Microsoft.DefaultPrograms /page pageDefaultProgram\pageAdvancedSettings?pszAppName=%PORTABLE_FIREFOX_NAME_SHORT%

CLS && ECHO.&ECHO Done! &&PAUSE>NUL & EXIT


:UnsetDefaultBrowser

reg delete "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%HTML" /f

reg delete "HKLM\Software\Classes\%PORTABLE_FIREFOX_NAME_SHORT%URL" /f

reg delete "HKLM\Software\RegisteredApplications" /v "%PORTABLE_FIREFOX_NAME_SHORT%" /f

reg delete "HKLM\Software\Clients\StartMenuInternet\%PORTABLE_FIREFOX_NAME_SHORT%" /f

CLS && ECHO.&ECHO Done! &&PAUSE>NUL & EXIT
