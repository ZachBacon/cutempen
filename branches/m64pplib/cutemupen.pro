# -------------------------------------------------
# Project created by QtCreator 2010-02-26T16:25:43
# -------------------------------------------------
# ## The settings you have to adapt to your configuration:
# On Windows, the paths to Mupen64Plus, Zlib  and SDL include files
win32:INCLUDEPATH += C:\dev\mupen64plus-bundle-src-1.99.4\source\mupen64plus-core\src\api \
    C:\dev\include \
    C:\dev\SDL-1.2.15\include

# On Windows, the path to Zlib and SDL libraries
windows:LIBS += C:\dev\lib\zlib.lib \
    C:\dev\SDL-1.2.15\lib\libSDL.dll.a

# On other systems (GNU/Linux, Mac OS X, ...), the path to Mupen64Plus API files
!win32:INCLUDEPATH += /usr/include/mupen64plus

# -------------------------------------------------
CONFIG += debug
TARGET = cutemupen
TEMPLATE = app
SOURCES += inputdialog.cpp \
    plugindialog.cpp \
    settings.cpp \
    main.cpp \
    mainwindow.cpp \
    paths.cpp \
    m64pcom.cpp \
    mupen64plusplus/MupenAPI.c \
    mupen64plusplus/MupenAPIpp.cpp \
    mupen64plusplus/plugin.c \
    quazip/ioapi.c \
    quazip/quazip.cpp \
    quazip/quazipfile.cpp \
    quazip/quazipnewinfo.cpp \
    quazip/unzip.c \
    quazip/zip.c
HEADERS += inputdialog.h \
    plugindialog.h \
    mainwindow.h \
    version.h \
    mupen64plusplus/MupenAPI.h \
    mupen64plusplus/MupenAPIpp.h \
    mupen64plusplus/osal_dynamiclib.h \
    mupen64plusplus/osal_files.h \
    mupen64plusplus/osal_preproc.h \
    mupen64plusplus/plugin.h \
    mupen64plusplus/version.h \
    quazip/crypt.h \
    quazip/ioapi.h \
    quazip/quazip.h \
    quazip/quazipfile.h \
    quazip/quazipfileinfo.h \
    quazip/quazipnewinfo.h \
    quazip/unzip.h \
    quazip/zip.h
unix:LIBS += -lz \
    -lSDL \
    -ldl
FORMS += inputdialog.ui \
    plugindialog.ui \
    mainwindow.ui
win32:CONFIG += embed_manifest_exe
win32:SOURCES += mupen64plusplus/osal_files_win32.c \
    mupen64plusplus/osal_dynamiclib_win32.c
!win32:SOURCES += mupen64plusplus/osal_files_unix.c \
    mupen64plusplus/osal_dynamiclib_unix.c
RESOURCES += resources.qrc
TRANSLATIONS = lang/cutemupen_fr.ts
CODECFORSRC = UTF-8
