@echo off

objcopy -I binary -O pe-x86-64 --binary-architecture i386 all.res all_res.obj

lib /OUT:all_res.lib all_res.obj

dumpbin /symbols lib\all_res.lib

cl main.cpp /Fe:testris.exe /MD /I lib\include\ /link /SUBSYSTEM:WINDOWS /LIBPATH:lib all_res.lib glfw3.lib glew32s.lib opengl32.lib user32.lib gdi32.lib shell32.lib winmm.lib

