bcc32 %1 -c -ocfroute.obj -I..\src -DSQUISHCFS -I..\..\smapi -WC -DWIN -D__NT__ -DWINNT ..\src\cfroute.cpp
bcc32 %1 -c -ocfroute.obj -I..\src -DSQUISHCFS -I..\..\smapi -WC -DWIN -D__NT__ -DWINNT ..\src\buffer.cpp
bcc32 %1 -c -ocfroute.obj -I..\src -DSQUISHCFS -I..\..\smapi -WC -DWIN -D__NT__ -DWINNT ..\src\structs.cpp
bcc32 %1 -c -odirute.obj -I..\src -DSQUISHCFS -I..\..\smapi -WC -DWIN -D__NT__ -DWINNT ..\src\dirute.c
bcc32 %1 -c -odirute.obj -I..\src -DSQUISHCFS -I..\..\smapi -WC -DWIN -D__NT__ -DWINNT ..\src\fecfg145.c
bcc32 -L..\..\smapi -WC -ecfroute.exe cfroute.obj dirute.obj structs.obj buffer.obj smapibcw.lib
