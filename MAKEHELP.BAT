@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by LIGHT.HPJ. >hlp\light.hm
echo. >>hlp\light.hm
echo // Commands (ID_* and IDM_*) >>hlp\light.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>hlp\light.hm
echo. >>hlp\light.hm
echo // Prompts (IDP_*) >>hlp\light.hm
makehm IDP_,HIDP_,0x30000 resource.h >>hlp\light.hm
echo. >>hlp\light.hm
echo // Resources (IDR_*) >>hlp\light.hm
makehm IDR_,HIDR_,0x20000 resource.h >>hlp\light.hm
echo. >>hlp\light.hm
echo // Dialogs (IDD_*) >>hlp\light.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\light.hm
echo. >>hlp\light.hm
echo // Frame Controls (IDW_*) >>hlp\light.hm
makehm IDW_,HIDW_,0x50000 resource.h >>hlp\light.hm
REM -- Make help for Project LIGHT

echo Building Win32 Help files
call hc31 light.hpj
if exist windebug copy light.hlp windebug
if exist winrel copy light.hlp winrel
EndLocal
