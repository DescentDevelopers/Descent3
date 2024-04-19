@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by D3LAUNCH.HPJ. >"hlp\D3Launch.hm"
echo. >>"hlp\D3Launch.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\D3Launch.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\D3Launch.hm"
echo. >>"hlp\D3Launch.hm"
echo // Prompts (IDP_*) >>"hlp\D3Launch.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\D3Launch.hm"
echo. >>"hlp\D3Launch.hm"
echo // Resources (IDR_*) >>"hlp\D3Launch.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\D3Launch.hm"
echo. >>"hlp\D3Launch.hm"
echo // Dialogs (IDD_*) >>"hlp\D3Launch.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\D3Launch.hm"
echo. >>"hlp\D3Launch.hm"
echo // Frame Controls (IDW_*) >>"hlp\D3Launch.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\D3Launch.hm"
REM -- Make help for Project D3LAUNCH


echo Building Win32 Help files
start /wait "C:\Program Files\Help Workshop\hcw.exe" /C /E /M "hlp\D3Launch.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\D3Launch.hlp" goto :Error
if not exist "hlp\D3Launch.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\D3Launch.hlp" Debug
if exist Debug\nul copy "hlp\D3Launch.cnt" Debug
if exist Release\nul copy "hlp\D3Launch.hlp" Release
if exist Release\nul copy "hlp\D3Launch.cnt" Release
echo.
goto :done

:Error
echo hlp\D3Launch.hpj(1) : error: Problem encountered creating help file

:done
echo.
