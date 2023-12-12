@ECHO OFF & PUSHD "%CD%" && CD /D "%~dp0"
>NUL 2>&1 REG.exe query "HKU\S-1-5-19" || (
    ECHO SET UAC = CreateObject^("Shell.Application"^) > "%TEMP%\Getadmin.vbs"
    ECHO UAC.ShellExecute "%~f0", "%1", "", "runas", 1 >> "%TEMP%\Getadmin.vbs"
    "%TEMP%\Getadmin.vbs"
    DEL /f /q "%TEMP%\Getadmin.vbs" 2>NUL
    Exit /b
)

set "PORTABLE_FIREFOX_NAME=PortableFirefox"
set "PORTABLE_FIREFOX_NAME_SHORT=PortableFirefox"
set "PORTABLE_FIREFOX_DESCRIPTION=Firefox portable launcher and updater"
set "PORTABLE_FIREFOX_PATH=%~dp0PortableFirefox.exe"
set "PORTABLE_FIREFOX_ICON=\"%PORTABLE_FIREFOX_PATH%\",0"
set "PORTABLE_FIREFOX_ARGS=\"%PORTABLE_FIREFOX_PATH%\" \"%%1\""

if not exist "%PORTABLE_FIREFOX_PATH%" (

	echo ERROR: "%PORTABLE_FIREFOX_PATH%" not found.

) else (

	reg add hklm /f>nul 2>&1

	if ERRORLEVEL 1 (

		echo ERROR: you have no privileges.

	) else (

		regedit /s "%~dp0RegistryCleaner.reg"

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
	)
)

pause
