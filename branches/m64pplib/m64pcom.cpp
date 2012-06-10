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
//extern QStringList parameterList;
extern PluginDialog* pDialog;
extern InputDialog* inputDialog;


m64p_error MainWindow::InitMupen64()
{
    m_api = new Mupen64PlusPlus(Mupen64Library.toLocal8Bit().constData(),
                                Mupen64PluginDir.toLocal8Bit().constData(),
                                //ui->cb_GfxPlugin->currentText().toLocal8Bit().constData(),
                                //"mupen64plus-video-rice",
                                "mupen64plus-video-arachnoid",
                                ui->cb_SndPlugin->currentText().toLocal8Bit().constData(),
                                ui->cb_InpPlugin->currentText().toLocal8Bit().constData(),
                                ui->cb_RspPlugin->currentText().toLocal8Bit().constData());
    if (m_api)
    {
        m_api->getConfigContents();
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
    qint64 romlength = file.size();
    /*unsigned*/ char *FILE_buffer = (char*)file.map(0, romlength);
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
                //m_api->loadRom (zippedFile.usize(), ROM_buffer);
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
        LoadRom (romlength, ROM_buffer);
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
        //(*CoreShutdown)();
        DetachCoreLib();
        return false;
    }
    ::attachPlugins();
    return true;
}

m64p_error MainWindow::DetachCoreLib()
{
  return ::DetachCoreLib();
}

m64p_error MainWindow::OpenConfigurationHandles(void)
{

}

m64p_error MainWindow::SaveConfigurationOptions(void)
{
#if 0
    /* if shared data directory was given on the command line, write it into the config file */
    if (l_DataDirPath != NULL)
        (*ConfigSetParameter)(l_ConfigCore, "SharedDataPath", M64TYPE_STRING, l_DataDirPath);

    /* if any plugin filepaths were given on the command line, write them into the config file */
    if (g_PluginDir != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "PluginDir", M64TYPE_STRING, g_PluginDir);
    if (g_GfxPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "VideoPlugin", M64TYPE_STRING, g_GfxPlugin);
    if (g_AudioPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "AudioPlugin", M64TYPE_STRING, g_AudioPlugin);
    if (g_InputPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "InputPlugin", M64TYPE_STRING, g_InputPlugin);
    if (g_RspPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "RspPlugin", M64TYPE_STRING, g_RspPlugin);

    return (*ConfigSaveFile)();
#endif
}

/* ============================================================================= */

m64p_handle GetSectionHandle (const char* name)
{

  m64p_handle handle;
  handle = 0;
#if 0
  m64p_error result = (*ConfigOpenSection)(name, &handle);
  if (result != M64ERR_SUCCESS)
    qDebug () << "GetSectionHandle: unable to get" << name << "section !";
#endif
  return handle;
}

#if 0
void ParameterListCallback(void* sectionHandle, const char* ParamName, m64p_type ParamType)
{
    m64p_handle* section = (m64p_handle*)sectionHandle;

    bool isInput = false;
    m64p_handle inpH1 = GetSectionHandle ("Input-SDL-Control1");
    m64p_handle inpH2 = GetSectionHandle ("Input-SDL-Control2");
    m64p_handle inpH3 = GetSectionHandle ("Input-SDL-Control3");
    m64p_handle inpH4 = GetSectionHandle ("Input-SDL-Control4");
    if ((inpH1 == section) || (inpH2 == section) || (inpH3 == section) || (inpH4 == section))
      isInput = true;

    bool boolValue = true;
    int intValue = -1;
    float floatValue = -1.0f;
    char* stringValue;
    switch (ParamType)
    {
      case M64TYPE_BOOL:
        boolValue = (*ConfigGetParamBool)(section, ParamName);
        if (isInput)
          inputDialog->AddParameter(ParamName, ParamType, &boolValue);
        else
          pDialog->AddParameter(ParamName, ParamType, &boolValue);
        break;
      case M64TYPE_FLOAT:
        floatValue = (*ConfigGetParamFloat)(section, ParamName);
        if (isInput)
          inputDialog->AddParameter(ParamName, ParamType, &floatValue);
        else
          pDialog->AddParameter(ParamName, ParamType, &floatValue);
        break;
      case M64TYPE_INT:
        intValue = (*ConfigGetParamInt)(section, ParamName);
        if (isInput)
          inputDialog->AddParameter(ParamName, ParamType, &floatValue);
        else
          pDialog->AddParameter(ParamName, ParamType, &intValue);
        break;
      case M64TYPE_STRING:
        stringValue = (char*)(*ConfigGetParamString)(section, ParamName);
        if (isInput)
          inputDialog->AddParameter(ParamName, ParamType, &floatValue);
        else
          pDialog->AddParameter(ParamName, ParamType, stringValue);
        break;
    }
}

void SectionListCallback(void* context, const char* section)
{
    //(void)context;
    QStringList* configSections;
    configSections = (QStringList*)context;
    //qDebug () << section;
    configSections->append (section);
}
#endif

void MainWindow::GetConfigurationSections ()
{
#if 0
  configSections.clear();
  m64p_error err;
  err = (*ConfigListSections)((void*)&configSections, SectionListCallback);
  if (err != M64ERR_SUCCESS)
    qDebug () << "GetConfigurationSections: unable to list sections !";
#endif
}

m64p_error MainWindow::GetSectionParameters (const char* sectionName)
{
#if 0
  m64p_handle sHandle = GetSectionHandle (sectionName);
  m64p_error res = (*ConfigListParameters)(sHandle, sHandle, ParameterListCallback);
  if (res != M64ERR_SUCCESS)
    qDebug () << "ConfigListParameters failed, return code = " << res;

  return res;
#endif
}
