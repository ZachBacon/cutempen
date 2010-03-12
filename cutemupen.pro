# -------------------------------------------------
# Project created by QtCreator 2010-02-26T16:25:43
# -------------------------------------------------
CONFIG += console \
    release
TARGET = cutemupen
TEMPLATE = app
SOURCES += settings.cpp \
    m64p/core_interface.c \
    m64p/plugin.cpp \
    main.cpp \
    mainwindow.cpp \
    paths.cpp
HEADERS += m64p/core_interface.h \
    m64p/plugin.h \
    mainwindow.h \
    osal/osal_dynamiclib.h \
    osal/osal_files.h \
    osal/osal_preproc.h \
    version.h
INCLUDEPATH += /usr/include/mupen64plus
FORMS += mainwindow.ui \
    mainwindow.ui
win32:CONFIG += embed_manifest_exe
win32:SOURCES += osal/osal_files_win32.cpp \
    osal/osal_dynamiclib_win32.c
!win32:SOURCES += osal/osal_files_unix.c \
    osal/osal_dynamiclib_unix.c
RESOURCES += resources.qrc \
    resources.qrc
