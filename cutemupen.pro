# -------------------------------------------------
# Project created by QtCreator 2010-02-26T16:25:43
# -------------------------------------------------
# ## The settings you have to adapt to your configuration:
# On Windows, the paths to Mupen64Plus API and ZLib include files
win32:INCLUDEPATH += C:\dev\mupen64plus-bundle-src-1.99.4\source\mupen64plus-core\src\api \
    C:\dev\include

# On Windows, the path to ZLib library
windows:LIBS += C:\dev\lib\zlib.lib

# On other systems (GNU/Linux, OSX, ...), the path to Mupen64Plus API files
!win32:INCLUDEPATH += /usr/include/mupen64plus

# -------------------------------------------------
CONFIG += release
TARGET = cutemupen
TEMPLATE = app
SOURCES += inputdialog.cpp \
    plugindialog.cpp \
    m64pcom.cpp \
    settings.cpp \
    m64p/core_interface.c \
    m64p/plugin.cpp \
    main.cpp \
    mainwindow.cpp \
    paths.cpp \
    quazip/ioapi.c \
    quazip/quazip.cpp \
    quazip/quazipfile.cpp \
    quazip/quazipnewinfo.cpp \
    quazip/unzip.c \
    quazip/zip.c
HEADERS += inputdialog.h \
    plugindialog.h \
    m64p/core_interface.h \
    m64p/plugin.h \
    mainwindow.h \
    osal/osal_dynamiclib.h \
    osal/osal_files.h \
    osal/osal_preproc.h \
    version.h \
    quazip/crypt.h \
    quazip/ioapi.h \
    quazip/quazip.h \
    quazip/quazipfile.h \
    quazip/quazipfileinfo.h \
    quazip/quazipnewinfo.h \
    quazip/unzip.h \
    quazip/zip.h
unix:LIBS += -lz \
    -ldl
FORMS += inputdialog.ui \
    plugindialog.ui \
    mainwindow.ui
win32:CONFIG += embed_manifest_exe
win32:SOURCES += osal/osal_files_win32.cpp \
    osal/osal_dynamiclib_win32.c
!win32:SOURCES += osal/osal_files_unix.c \
    osal/osal_dynamiclib_unix.c
RESOURCES += resources.qrc
TRANSLATIONS = lang/cutemupen_fr.ts
CODECFORSRC = UTF-8
