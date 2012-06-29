/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   CuteMupen - m64pcom.cpp                                               *
 *   CuteMupen homepage: http://cutemupen.sourceforge.net/                 *
 *   Copyright (C) 2009 Richard Goedeken                                   *
 *   Copyright (C) 2010 Vincent Knecht <vknecht@users.sourceforge.net>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "version.h"
#include "plugindialog.h"
#include "inputdialog.h"

#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QDebug>

#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include "mupen64plusplus/plugin.h"
#include "mupen64plusplus/MupenAPIpp.h"

extern QString* logLine;
extern QStringList* logList;
extern PluginDialog* pluginDialog;
extern InputDialog* inputDialog;

m64p_error MainWindow::InitMupen64()
{
    // Try a reasonable default for plugin dir
    Mupen64PluginDir = Mupen64Library.left(
                Mupen64Library.lastIndexOf(OSAL_DIR_SEPARATOR)).toLocal8Bit().constData();
    UpdateM64PluginDir();
    m_api = new Mupen64PlusPlus(Mupen64Library.toLocal8Bit().constData(),
                                Mupen64PluginDir.toLocal8Bit().constData(),
                                "mupen64plus-video-rice",
                                "mupen64plus-audio-sdl",
                                "mupen64plus-input-sdl",
                                "mupen64plus-rsp-hle");
    if (m_api)
    {
        configSections = m_api->getConfigContents();
        RestoreSettings();
        qDebug() << "Initialized M64+ and restored settings.";
        return M64ERR_SUCCESS;
    }
    return M64ERR_NOT_INIT;
}

bool MainWindow::LoadFile(QString & ROMFile)
{
    QFile file (ROMFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("Error opening ROM file"),
            tr("Error while trying to load ROM file:\n") + ROMFile);
        return false;
    }
    qint64 fileSize = file.size();
    /*unsigned*/ char *FILE_buffer = (char*)file.map(0, fileSize);
    /*unsigned*/ char *ROM_buffer = 0;

    // Check ZIP file signature
    if ((FILE_buffer[0] == 'P') && (FILE_buffer[1] == 'K'))
    {
        QuaZip zip (ROMFile);
        zip.open(QuaZip::mdUnzip);
        QuaZipFile zippedFile (&zip);
        for (bool more=zip.goToFirstFile(); more; more=zip.goToNextFile())
        {
            QString fileName;
            if (zippedFile.open(QIODevice::ReadOnly))
                fileName = zippedFile.getActualFileName();
            else
                continue;

            if (fileName.endsWith("64"))
            {
                QByteArray bArray = zippedFile.readAll();
                ROM_buffer = (char*)bArray.data();
                LoadRom (zippedFile.usize(), ROM_buffer);
                zippedFile.close();
                // @TODO : handle more-than-one-ROM-inna-zip more gracefuly ?
                // For now, just handle the first one...
                break;
            }
            zippedFile.close();
        }
        zip.close();
    }
    else
    {
        // Not a ZIP file, load it directly
        ROM_buffer = FILE_buffer;
        LoadRom (fileSize, ROM_buffer);
    }

    // the core copies the ROM image, so we can release this buffer immediately
    file.unmap((unsigned char*)ROM_buffer);
    file.close();
    return true;
}

bool MainWindow::LoadRom (qint64 romlength, char* buffer)
{
    // Try to load the ROM image into the core
    if (openRom((int)romlength, buffer) != M64ERR_SUCCESS)
    {
        logLine->sprintf("Error: core failed to open ROM image.");
        logList->append(*logLine);
        ::DetachCoreLib();
        return false;
    }
    ::attachPlugins();
    return true;
}

/* ============================================================================= */

ConfigSection* MainWindow::GetSection (const char* name)
{
    for (size_t idx = 0; idx < configSections.size(); idx++)
    {
        if (!strcmp(configSections[idx].m_section_name.c_str(), name))
            return &configSections[idx];
    }
    return 0;
}

ConfigSection* MainWindow::GetSection (m64p_handle handle)
{
    for (size_t idx = 0; idx < configSections.size(); idx++)
    {
        if (configSections[idx].m_handle == handle)
            return &configSections[idx];
    }
    return 0;
}
