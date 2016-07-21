# Microsoft Developer Studio Project File - Name="LIGHT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LIGHT - Win32 Profile
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Light.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Light.mak" CFG="LIGHT - Win32 Profile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIGHT - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "LIGHT - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LIGHT - Win32 Profile" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIGHT - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\WinDebug"
# PROP Intermediate_Dir ".\WinDebug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp2 /MTd /W3 /Gm /GX /ZI /Od /I ".\Sup" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_OS_NT_" /D "_MBCS" /D "HTTPS" /D "HAVE_THREADS" /D "HAVE_IO_H" /D "VISUALCPP" /FR /Yu"stdafx.h" /FD /c
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 version.lib /nologo /stack:0x200000 /subsystem:windows /map /debug /machine:I386 /out:"d:\fipstest\Light.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "LIGHT - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\WinRel"
# PROP Intermediate_Dir ".\WinRel"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp2 /MT /W3 /GX /O2 /I ".\Sup" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_OS_NT_" /D "_MBCS" /D "HTTPS" /D "HAVE_THREADS" /D "HAVE_IO_H" /D "VISUALCPP" /FR /Yu"stdafx.h" /FD /c
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 version.lib /nologo /stack:0x200000 /subsystem:windows /machine:I386 /out:"d:\fipstest\Fips.exe"
# SUBTRACT LINK32 /profile /incremental:yes /map /debug

!ELSEIF  "$(CFG)" == "LIGHT - Win32 Profile"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LIGHT___Win32_Profile"
# PROP BASE Intermediate_Dir "LIGHT___Win32_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LIGHT___Win32_Profile"
# PROP Intermediate_Dir "LIGHT___Win32_Profile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp2 /MTd /W3 /Gm /GX /ZI /Od /I ".\Sup" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_OS_NT_" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp2 /MTd /W3 /Gm /GX /ZI /Od /I ".\Sup" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_OS_NT_" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /mktyplib203
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /stack:0x200000 /subsystem:windows /map /debug /machine:I386 /out:"d:\fipstest\Light.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 version.lib /nologo /stack:0x200000 /subsystem:windows /profile /map /debug /machine:I386 /out:"d:\fipstest\Light.exe"

!ENDIF 

# Begin Target

# Name "LIGHT - Win32 Debug"
# Name "LIGHT - Win32 Release"
# Name "LIGHT - Win32 Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "Binkd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\binkd\binkd.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\binlog.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\bsy.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\client.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\crypt.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\dirwin32.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\ftnaddr.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\ftndom.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\ftnnode.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\ftnq.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\getfree.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\getw.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\https.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\inbound.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\iptools.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\md5b.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\prothlp.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\protocol.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\readcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\readflo.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\run.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\sem.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\server.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\srif.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\TCPErr.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\tools.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\WSock.cpp
# End Source File
# Begin Source File

SOURCE=.\binkd\xalloc.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\adrbook.cpp
# End Source File
# Begin Source File

SOURCE=.\again.cpp
# End Source File
# Begin Source File

SOURCE=.\al_exist.cpp
# End Source File
# Begin Source File

SOURCE=.\arcmail.cpp
# End Source File
# Begin Source File

SOURCE=.\areasel.cpp
# End Source File
# Begin Source File

SOURCE=.\badxfer.cpp
# End Source File
# Begin Source File

SOURCE=.\bbsarea.cpp
# End Source File
# Begin Source File

SOURCE=.\bbsgrp.cpp
# End Source File
# Begin Source File

SOURCE=.\bbsmain.cpp
# End Source File
# Begin Source File

SOURCE=.\bbsstuff.cpp
# End Source File
# Begin Source File

SOURCE=.\bbsuser.cpp
# End Source File
# Begin Source File

SOURCE=.\bmpview.cpp
# End Source File
# Begin Source File

SOURCE=.\mime\Cb64endec.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_aaka.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_adef.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_advn.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_apno.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_area.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_boss.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_carb.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_col.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_conv.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_dir.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_edit.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_fax.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_gate.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_inco.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_mode.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_mula.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_mult.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_nfll.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_node.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_othr.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_pack.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_ring.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_rmon.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_scr.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_serv.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_tics.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_time.cpp
# End Source File
# Begin Source File

SOURCE=.\cfg_tras.cpp
# End Source File
# Begin Source File

SOURCE=.\cfgexfr.cpp
# End Source File
# Begin Source File

SOURCE=.\cfgmagic.cpp
# End Source File
# Begin Source File

SOURCE=.\cfgrpath.cpp
# End Source File
# Begin Source File

SOURCE=.\cfgsound.cpp
# End Source File
# Begin Source File

SOURCE=.\cfido.cpp
# End Source File
# Begin Source File

SOURCE=.\cha_tmpl.cpp
# End Source File
# Begin Source File

SOURCE=.\cha_xfer.cpp
# End Source File
# Begin Source File

SOURCE=.\changed.cpp
# End Source File
# Begin Source File

SOURCE=.\charging.cpp
# End Source File
# Begin Source File

SOURCE=.\choosas.cpp
# End Source File
# Begin Source File

SOURCE=.\costanal.cpp
# End Source File
# Begin Source File

SOURCE=.\costbase.cpp
# End Source File
# Begin Source File

SOURCE=.\costuser.cpp
# End Source File
# Begin Source File

SOURCE=.\crospost.cpp
# End Source File
# Begin Source File

SOURCE=.\csobj.CPP
# End Source File
# Begin Source File

SOURCE=.\cuuendec.cpp
# End Source File
# Begin Source File

SOURCE=.\database.cpp
# End Source File
# Begin Source File

SOURCE=.\dbcon.cpp
# End Source File
# Begin Source File

SOURCE=.\det_nl.cpp
# End Source File
# Begin Source File

SOURCE=.\detmail.cpp
# End Source File
# Begin Source File

SOURCE=.\detpurg.cpp
# End Source File
# Begin Source File

SOURCE=.\dettoss.cpp
# End Source File
# Begin Source File

SOURCE=.\dibapi.cpp
# End Source File
# Begin Source File

SOURCE=.\ecolst.cpp
# End Source File
# Begin Source File

SOURCE=.\emergenc.cpp
# End Source File
# Begin Source File

SOURCE=.\error.cpp
# End Source File
# Begin Source File

SOURCE=.\events.cpp
# End Source File
# Begin Source File

SOURCE=.\evpump.cpp
# End Source File
# Begin Source File

SOURCE=.\exbrow.cpp
# End Source File
# Begin Source File

SOURCE=.\exsearch.cpp
# End Source File
# Begin Source File

SOURCE=.\fax.cpp
# End Source File
# Begin Source File

SOURCE=.\filereq.cpp
# End Source File
# Begin Source File

SOURCE=.\FListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\floating.cpp
# End Source File
# Begin Source File

SOURCE=.\hatch.cpp
# End Source File
# Begin Source File

SOURCE=.\import.cpp
# End Source File
# Begin Source File

SOURCE=.\impwhat.cpp
# End Source File
# Begin Source File

SOURCE=.\inafound.cpp
# End Source File
# Begin Source File

SOURCE=.\info_adr.cpp
# End Source File
# Begin Source File

SOURCE=.\inouthis.cpp
# End Source File
# Begin Source File

SOURCE=.\insmm.cpp
# End Source File
# Begin Source File

SOURCE=.\kludges.cpp
# End Source File
# Begin Source File

SOURCE=.\MathParser\Lexer.cpp
# End Source File
# Begin Source File

SOURCE=.\Light.cpp
# End Source File
# Begin Source File

SOURCE=.\light.rc
# End Source File
# Begin Source File

SOURCE=.\lightdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\logdisp.cpp
# End Source File
# Begin Source File

SOURCE=.\logopti.cpp
# End Source File
# Begin Source File

SOURCE=.\mailhan.cpp
# End Source File
# Begin Source File

SOURCE=.\manual.cpp
# End Source File
# Begin Source File

SOURCE=.\marksel.cpp
# End Source File
# Begin Source File

SOURCE=.\MathParser\MathParser.CPP
# End Source File
# Begin Source File

SOURCE=.\msgbox.cpp
# End Source File
# Begin Source File

SOURCE=.\myedit.cpp
# End Source File
# Begin Source File

SOURCE=.\myeditfr.cpp
# End Source File
# Begin Source File

SOURCE=.\mylist.cpp
# End Source File
# Begin Source File

SOURCE=.\myowlist.cpp
# End Source File
# Begin Source File

SOURCE=.\mystrlst.cpp
# End Source File
# Begin Source File

SOURCE=.\net.cpp
# End Source File
# Begin Source File

SOURCE=.\newmail.cpp
# End Source File
# Begin Source File

SOURCE=.\nodelist.cpp
# End Source File
# Begin Source File

SOURCE=.\nodesel.cpp
# End Source File
# Begin Source File

SOURCE=.\nt_debug.cpp
# End Source File
# Begin Source File

SOURCE=.\outbound.cpp
# End Source File
# Begin Source File

SOURCE=.\owlist.cpp
# End Source File
# Begin Source File

SOURCE=.\pkt_view.cpp
# End Source File
# Begin Source File

SOURCE=.\SUP\pr_common.cpp
# End Source File
# Begin Source File

SOURCE=.\SUP\Pr_hydra.cpp
# End Source File
# Begin Source File

SOURCE=.\SUP\Pr_xfer.cpp
# End Source File
# Begin Source File

SOURCE=.\SUP\pr_zmod.cpp
# End Source File
# Begin Source File

SOURCE=.\quickset.cpp
# End Source File
# Begin Source File

SOURCE=.\rbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# Begin Source File

SOURCE=.\receiver.cpp
# End Source File
# Begin Source File

SOURCE=.\reqsel.cpp
# End Source File
# Begin Source File

SOURCE=.\resultfm.cpp
# End Source File
# Begin Source File

SOURCE=.\rfilter.cpp
# End Source File
# Begin Source File

SOURCE=.\SAPrefsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SAPrefsStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\SAPrefsSubDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SButton.cpp
# End Source File
# Begin Source File

SOURCE=.\screenpo.cpp
# End Source File
# Begin Source File

SOURCE=.\script.cpp
# End Source File
# Begin Source File

SOURCE=.\search.cpp
# End Source File
# Begin Source File

SOURCE=.\searepl.cpp
# End Source File
# Begin Source File

SOURCE=.\sedesta.cpp
# End Source File
# Begin Source File

SOURCE=.\sel_freq.cpp
# End Source File
# Begin Source File

SOURCE=.\selevbos.cpp
# End Source File
# Begin Source File

SOURCE=.\simple.cpp
# End Source File
# Begin Source File

SOURCE=.\simppoll.cpp
# End Source File
# Begin Source File

SOURCE=.\splash.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\MathParser\StrMap.cpp
# End Source File
# Begin Source File

SOURCE=.\sub_adj.cpp
# End Source File
# Begin Source File

SOURCE=.\MathParser\SymTable.cpp
# End Source File
# Begin Source File

SOURCE=.\tapisel.cpp
# End Source File
# Begin Source File

SOURCE=.\ticedit.cpp
# End Source File
# Begin Source File

SOURCE=.\timed.cpp
# End Source File
# Begin Source File

SOURCE=.\tosslib.cpp
# End Source File
# Begin Source File

SOURCE=.\traffic.cpp
# End Source File
# Begin Source File

SOURCE=.\trash.cpp
# End Source File
# Begin Source File

SOURCE=.\trepl.cpp
# End Source File
# Begin Source File

SOURCE=.\trmansi.cpp
# End Source File
# Begin Source File

SOURCE=.\txttemp.cpp
# End Source File
# Begin Source File

SOURCE=.\u_crc.cpp
# End Source File
# Begin Source File

SOURCE=.\valarea.cpp
# End Source File
# Begin Source File

SOURCE=.\wiedinf.cpp
# End Source File
# Begin Source File

SOURCE=.\writedit.cpp
# End Source File
# Begin Source File

SOURCE=.\writmail.cpp
# End Source File
# Begin Source File

SOURCE=.\ntdebug.obj
# End Source File
# Begin Source File

SOURCE=.\sup\Win32_De\PROTOCOL.lib
# End Source File
# Begin Source File

SOURCE=.\sup\Win32_D0\SUPERCOM.lib
# End Source File
# Begin Source File

SOURCE=.\WINMM.LIB
# End Source File
# Begin Source File

SOURCE=.\WSOCK32.LIB
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\adrbook.h
# End Source File
# Begin Source File

SOURCE=.\again.h
# End Source File
# Begin Source File

SOURCE=.\al_exist.h
# End Source File
# Begin Source File

SOURCE=.\areasel.h
# End Source File
# Begin Source File

SOURCE=.\badxfer.h
# End Source File
# Begin Source File

SOURCE=.\bbsarea.h
# End Source File
# Begin Source File

SOURCE=.\bbsgrp.h
# End Source File
# Begin Source File

SOURCE=.\bbsmain.h
# End Source File
# Begin Source File

SOURCE=.\bbsuser.h
# End Source File
# Begin Source File

SOURCE=.\bmpview.h
# End Source File
# Begin Source File

SOURCE=.\mime\CB64ENDEC.H
# End Source File
# Begin Source File

SOURCE=.\cfg_aaka.h
# End Source File
# Begin Source File

SOURCE=.\cfg_adef.h
# End Source File
# Begin Source File

SOURCE=.\cfg_advn.h
# End Source File
# Begin Source File

SOURCE=.\cfg_apno.h
# End Source File
# Begin Source File

SOURCE=.\cfg_area.h
# End Source File
# Begin Source File

SOURCE=.\cfg_boss.h
# End Source File
# Begin Source File

SOURCE=.\cfg_carb.h
# End Source File
# Begin Source File

SOURCE=.\cfg_col.h
# End Source File
# Begin Source File

SOURCE=.\cfg_conv.h
# End Source File
# Begin Source File

SOURCE=.\cfg_dir.h
# End Source File
# Begin Source File

SOURCE=.\cfg_edit.h
# End Source File
# Begin Source File

SOURCE=.\cfg_fax.h
# End Source File
# Begin Source File

SOURCE=.\cfg_gate.h
# End Source File
# Begin Source File

SOURCE=.\cfg_icon.h
# End Source File
# Begin Source File

SOURCE=.\cfg_inco.h
# End Source File
# Begin Source File

SOURCE=.\cfg_mode.h
# End Source File
# Begin Source File

SOURCE=.\cfg_mula.h
# End Source File
# Begin Source File

SOURCE=.\cfg_mult.h
# End Source File
# Begin Source File

SOURCE=.\cfg_nfll.h
# End Source File
# Begin Source File

SOURCE=.\cfg_node.h
# End Source File
# Begin Source File

SOURCE=.\cfg_othr.h
# End Source File
# Begin Source File

SOURCE=.\cfg_pack.h
# End Source File
# Begin Source File

SOURCE=.\cfg_ring.h
# End Source File
# Begin Source File

SOURCE=.\cfg_rmon.h
# End Source File
# Begin Source File

SOURCE=.\cfg_scr.h
# End Source File
# Begin Source File

SOURCE=.\cfg_serv.h
# End Source File
# Begin Source File

SOURCE=.\cfg_tics.h
# End Source File
# Begin Source File

SOURCE=.\cfg_time.h
# End Source File
# Begin Source File

SOURCE=.\cfg_tras.h
# End Source File
# Begin Source File

SOURCE=.\cfgexfr.h
# End Source File
# Begin Source File

SOURCE=.\cfgmagic.h
# End Source File
# Begin Source File

SOURCE=.\cfgrpath.h
# End Source File
# Begin Source File

SOURCE=.\cfgsound.h
# End Source File
# Begin Source File

SOURCE=.\cfido.h
# End Source File
# Begin Source File

SOURCE=.\cha_tmpl.h
# End Source File
# Begin Source File

SOURCE=.\cha_xfer.h
# End Source File
# Begin Source File

SOURCE=.\changed.h
# End Source File
# Begin Source File

SOURCE=.\choosas.h
# End Source File
# Begin Source File

SOURCE=.\costanal.h
# End Source File
# Begin Source File

SOURCE=.\costbase.h
# End Source File
# Begin Source File

SOURCE=.\costuser.h
# End Source File
# Begin Source File

SOURCE=.\crospost.h
# End Source File
# Begin Source File

SOURCE=.\csobj.h
# End Source File
# Begin Source File

SOURCE=.\cuuendec.h
# End Source File
# Begin Source File

SOURCE=.\det_nl.h
# End Source File
# Begin Source File

SOURCE=.\detmail.h
# End Source File
# Begin Source File

SOURCE=.\detpurg.h
# End Source File
# Begin Source File

SOURCE=.\dettoss.h
# End Source File
# Begin Source File

SOURCE=.\dibapi.h
# End Source File
# Begin Source File

SOURCE=.\ecolst.h
# End Source File
# Begin Source File

SOURCE=.\emergenc.h
# End Source File
# Begin Source File

SOURCE=.\events.h
# End Source File
# Begin Source File

SOURCE=.\exbrow.h
# End Source File
# Begin Source File

SOURCE=.\exsearch.h
# End Source File
# Begin Source File

SOURCE=.\faxinc.h
# End Source File
# Begin Source File

SOURCE=.\filereq.h
# End Source File
# Begin Source File

SOURCE=.\FListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\floating.h
# End Source File
# Begin Source File

SOURCE=.\gcomm.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\hatch.h
# End Source File
# Begin Source File

SOURCE=.\impwhat.h
# End Source File
# Begin Source File

SOURCE=.\inafound.h
# End Source File
# Begin Source File

SOURCE=.\info_adr.h
# End Source File
# Begin Source File

SOURCE=.\inouthis.h
# End Source File
# Begin Source File

SOURCE=.\insmm.h
# End Source File
# Begin Source File

SOURCE=.\kludges.h
# End Source File
# Begin Source File

SOURCE=.\MathParser\Lexer.h
# End Source File
# Begin Source File

SOURCE=.\light.h
# End Source File
# Begin Source File

SOURCE=.\lightdlg.h
# End Source File
# Begin Source File

SOURCE=.\logdisp.h
# End Source File
# Begin Source File

SOURCE=.\logopti.h
# End Source File
# Begin Source File

SOURCE=.\manual.h
# End Source File
# Begin Source File

SOURCE=.\marksel.h
# End Source File
# Begin Source File

SOURCE=.\MathParser\MathParser.h
# End Source File
# Begin Source File

SOURCE=.\msgbox.h
# End Source File
# Begin Source File

SOURCE=.\myedit.h
# End Source File
# Begin Source File

SOURCE=.\myeditfr.h
# End Source File
# Begin Source File

SOURCE=.\mylist.h
# End Source File
# Begin Source File

SOURCE=.\myowlist.h
# End Source File
# Begin Source File

SOURCE=.\mystrlst.h
# End Source File
# Begin Source File

SOURCE=.\newmail.h
# End Source File
# Begin Source File

SOURCE=.\nodesel.h
# End Source File
# Begin Source File

SOURCE=.\outbound.h
# End Source File
# Begin Source File

SOURCE=.\owlist.h
# End Source File
# Begin Source File

SOURCE=.\pkt_view.h
# End Source File
# Begin Source File

SOURCE=.\SUP\pr_common.h
# End Source File
# Begin Source File

SOURCE=.\SUP\PR_HYDRA.H
# End Source File
# Begin Source File

SOURCE=.\SUP\PR_XFER.H
# End Source File
# Begin Source File

SOURCE=.\SUP\pr_zmod.h
# End Source File
# Begin Source File

SOURCE=.\quickset.h
# End Source File
# Begin Source File

SOURCE=.\receiver.h
# End Source File
# Begin Source File

SOURCE=.\reqsel.h
# End Source File
# Begin Source File

SOURCE=.\resultfm.h
# End Source File
# Begin Source File

SOURCE=.\rfilter.h
# End Source File
# Begin Source File

SOURCE=.\SAPrefsDialog.h
# End Source File
# Begin Source File

SOURCE=.\SAPrefsStatic.h
# End Source File
# Begin Source File

SOURCE=.\SAPrefsSubDlg.h
# End Source File
# Begin Source File

SOURCE=.\SButton.h
# End Source File
# Begin Source File

SOURCE=.\search.h
# End Source File
# Begin Source File

SOURCE=.\searepl.h
# End Source File
# Begin Source File

SOURCE=.\sedesta.h
# End Source File
# Begin Source File

SOURCE=.\sel_freq.h
# End Source File
# Begin Source File

SOURCE=.\selevbos.h
# End Source File
# Begin Source File

SOURCE=.\simppoll.h
# End Source File
# Begin Source File

SOURCE=.\splash.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\MathParser\StrMap.h
# End Source File
# Begin Source File

SOURCE=.\structs.h
# End Source File
# Begin Source File

SOURCE=.\sub_adj.h
# End Source File
# Begin Source File

SOURCE=.\MathParser\SymTable.h
# End Source File
# Begin Source File

SOURCE=.\tapisel.h
# End Source File
# Begin Source File

SOURCE=.\ticedit.h
# End Source File
# Begin Source File

SOURCE=.\timed.h
# End Source File
# Begin Source File

SOURCE=.\traffic.h
# End Source File
# Begin Source File

SOURCE=.\trepl.h
# End Source File
# Begin Source File

SOURCE=.\txttemp.h
# End Source File
# Begin Source File

SOURCE=.\u_crc.h
# End Source File
# Begin Source File

SOURCE=.\valarea.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\wiedinf.h
# End Source File
# Begin Source File

SOURCE=.\writedit.h
# End Source File
# Begin Source File

SOURCE=.\writmail.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\abort.ico
# End Source File
# Begin Source File

SOURCE=.\RES\about.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\area.ico
# End Source File
# Begin Source File

SOURCE=.\RES\asave.ico
# End Source File
# Begin Source File

SOURCE=.\RES\coda.ico
# End Source File
# Begin Source File

SOURCE=.\RES\codansi.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\codansi.ico
# End Source File
# Begin Source File

SOURCE=.\RES\codo.ico
# End Source File
# Begin Source File

SOURCE=.\RES\codoem.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\codoem.ico
# End Source File
# Begin Source File

SOURCE=.\RES\com1_off.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\com1_on.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\com2_off.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\com2_on.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\empty.ico
# End Source File
# Begin Source File

SOURCE=.\RES\exit.ico
# End Source File
# Begin Source File

SOURCE=.\RES\file.ico
# End Source File
# Begin Source File

SOURCE=.\RES\filt.ico
# End Source File
# Begin Source File

SOURCE=.\RES\find.ico
# End Source File
# Begin Source File

SOURCE=.\RES\Fips.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\freq.ico
# End Source File
# Begin Source File

SOURCE=.\RES\http.cur
# End Source File
# Begin Source File

SOURCE=.\RES\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\RES\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\RES\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\include.hh
# End Source File
# Begin Source File

SOURCE=.\RES\isdn_off.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\isdn_on.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\isdnoff.ico
# End Source File
# Begin Source File

SOURCE=.\RES\isdnon.ico
# End Source File
# Begin Source File

SOURCE=.\RES\light.ico
# End Source File
# Begin Source File

SOURCE=.\RES\macro1.ico
# End Source File
# Begin Source File

SOURCE=.\RES\macro2.ico
# End Source File
# Begin Source File

SOURCE=.\RES\macro3.ico
# End Source File
# Begin Source File

SOURCE=.\RES\macro4.ico
# End Source File
# Begin Source File

SOURCE=.\RES\macro5.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mailer.avi
# End Source File
# Begin Source File

SOURCE=.\RES\mailer.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mailto.cur
# End Source File
# Begin Source File

SOURCE=.\RES\mod1off.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mod1on.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mod2off.ico
# End Source File
# Begin Source File

SOURCE=.\RES\mod2on.ico
# End Source File
# Begin Source File

SOURCE=.\RES\new.ico
# End Source File
# Begin Source File

SOURCE=.\RES\next.ico
# End Source File
# Begin Source File

SOURCE=.\RES\phoenix.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\phoenix.ico
# End Source File
# Begin Source File

SOURCE=.\RES\phoenix1.ico
# End Source File
# Begin Source File

SOURCE=.\RES\phoenix2.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\phoenix2.ico
# End Source File
# Begin Source File

SOURCE=.\RES\prev.ico
# End Source File
# Begin Source File

SOURCE=.\RES\purger.avi
# End Source File
# Begin Source File

SOURCE=.\RES\purger.ico
# End Source File
# Begin Source File

SOURCE=.\RES\quote.ico
# End Source File
# Begin Source File

SOURCE=.\RES\quotea.ico
# End Source File
# Begin Source File

SOURCE=.\RES\splash.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\tosser.avi
# End Source File
# Begin Source File

SOURCE=.\RES\tosser.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\RES\Light.exe.manifest
# End Source File
# End Target
# End Project
