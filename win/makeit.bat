bcc32 %1 -c -ocfroute.obj -I..\src -DSQUISHCFS -Ix:\smapi -WC -DWIN -D__NT__ ..\src\cfroute.cpp
bcc32 -Lx:\smapi -WC -ecfroute.exe cfroute.obj smapibcw.lib
