bcc32 %1 -c -ocfroute.obj -I..\src -DSQUISHCFS -Ix:\smapi -WC -DWIN -D__NT__ -DWINNT ..\src\cfroute.cpp
bcc32 %1 -c -odirute.obj -I..\src -DSQUISHCFS -Ix:\smapi -WC -DWIN -D__NT__ -DWINNT ..\src\dirute.c
bcc32 -Lx:\smapi -WC -ecfroute.exe cfroute.obj dirute.obj smapibcw.lib
