del *.obj
del *.sbr
del *.ncb
del *.opt
del *.plg
del *.00?
copy /b ntdebobj.cpp  ntdebug.obj
deltree /y windebug
deltree /y winrel

