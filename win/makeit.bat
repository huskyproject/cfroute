bcc32 %1 -c -ocfroute.obj -I..\src -DSQUISHCFS -I..\..\smapi -I..\.. -WC -DWIN -D__NT__ -DWINNT ..\src\cfroute.cpp
bcc32 %1 -c -obuffer.obj -I..\src -DSQUISHCFS -I..\..\smapi -I..\.. -WC -DWIN -D__NT__ -DWINNT ..\src\buffer.cpp
bcc32 %1 -c -ostructs.obj -I..\src -DSQUISHCFS -I..\..\smapi -I..\.. -WC -DWIN -D__NT__ -DWINNT ..\src\structs.cpp
bcc32 %1 -c -odirute.obj -I..\src -DSQUISHCFS -I..\..\smapi -I..\.. -WC -DWIN -D__NT__ -DWINNT ..\src\dirute.c
bcc32 %1 -c -ofecfg146 -I..\src -DSQUISHCFS -I..\..\smapi -I..\.. -WC -DWIN -D__NT__ -DWINNT ..\src\fecfg146.c
bcc32 -L..\..\smapi -WC -ecfroute.exe cfroute.obj dirute.obj structs.obj buffer.obj fecfg146.obj smapibcw.lib
bcc32 %1 -c -ofc2cfr.obj -I..\src -DSQUISHCFS -I..\..\smapi -I..\..\fidoconf -I..\.. -WC -DWIN -D__NT__ -DWINNT ..\src\fc2cfr.c
bcc32 -L..\..\smapi -L..\..\fidoconf -WC -efc2cfr.exe fc2cfr.obj fconfbcw.lib smapibcw.lib
