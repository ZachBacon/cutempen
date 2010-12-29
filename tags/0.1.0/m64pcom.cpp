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

extern "C" {
#include "osal/osal_preproc.h"
#include "osal/osal_dynamiclib.h"
#include "m64p_common.h"
#include "m64p_frontend.h"
#include "m64p_config.h"
#include "m64p/core_interface.h"
void DebugCallback(void *Context, int level, const char *message);
}

extern QString* logLine;
extern QStringList* logList;
//extern QStringList parameterList;
extern PluginDialog* pDialog;
extern InputDialog* inputDialog;

#include "m64p/plugin.h"

m64p_error MainWindow::InitMupen64()
{
    if (isCoreReady)
      return M64ERR_SUCCESS;

    // Load the core lib
    CoreHandle = NULL;
    m64p_error rval = M64ERR_INTERNAL;
    rval = osal_dynlib_open(&CoreHandle, Mupen64Library.toLocal8Bit().constData());
    if (rval != M64ERR_SUCCESS || CoreHandle == NULL)
    {
        QString res;
        res.sprintf("file: %s\nrval: %x\n", Mupen64Library.toLocal8Bit().constData(), rval);
        QMessageBox::critical(this, "Error loading Core library", res);
        return rval;
    }

    ptr_PluginGetVersion CoreVersionFunc;
    CoreVersionFunc = (ptr_PluginGetVersion) osal_dynlib_getproc(CoreHandle, "PluginGetVersion");
    if (CoreVersionFunc == NULL)
    {
        QMessageBox::critical(this, tr("PluginGetVersion"),
            tr("Couldn't find PluginGetVersion()"));
        osal_dynlib_close(CoreHandle);
        CoreHandle = NULL;
        return M64ERR_INPUT_INVALID;
    }

    CoreErrorMessage = (ptr_CoreErrorMessage) osal_dynlib_getproc(CoreHandle, "CoreErrorMessage");
    CoreStartup = (ptr_CoreStartup) osal_dynlib_getproc(CoreHandle, "CoreStartup");
    CoreShutdown = (ptr_CoreShutdown) osal_dynlib_getproc(CoreHandle, "CoreShutdown");
    CoreAttachPlugin = (ptr_CoreAttachPlugin) osal_dynlib_getproc(CoreHandle, "CoreAttachPlugin");
    CoreDetachPlugin = (ptr_CoreDetachPlugin) osal_dynlib_getproc(CoreHandle, "CoreDetachPlugin");
    CoreDoCommand = (ptr_CoreDoCommand) osal_dynlib_getproc(CoreHandle, "CoreDoCommand");
    CoreOverrideVidExt = (ptr_CoreOverrideVidExt) osal_dynlib_getproc(CoreHandle, "CoreOverrideVidExt");
    CoreAddCheat = (ptr_CoreAddCheat) osal_dynlib_getproc(CoreHandle, "CoreAddCheat");
    CoreCheatEnabled = (ptr_CoreCheatEnabled) osal_dynlib_getproc(CoreHandle, "CoreCheatEnabled");

    /* get function pointers to the configuration functions */
    ConfigListSections = (ptr_ConfigListSections) osal_dynlib_getproc(CoreHandle, "ConfigListSections");
    ConfigOpenSection = (ptr_ConfigOpenSection) osal_dynlib_getproc(CoreHandle, "ConfigOpenSection");
    ConfigListParameters = (ptr_ConfigListParameters) osal_dynlib_getproc(CoreHandle, "ConfigListParameters");
    ConfigSaveFile = (ptr_ConfigSaveFile) osal_dynlib_getproc(CoreHandle, "ConfigSaveFile");
    ConfigSetParameter = (ptr_ConfigSetParameter) osal_dynlib_getproc(CoreHandle, "ConfigSetParameter");
    ConfigGetParameter = (ptr_ConfigGetParameter) osal_dynlib_getproc(CoreHandle, "ConfigGetParameter");
    ConfigGetParameterType = (ptr_ConfigGetParameterType) osal_dynlib_getproc(CoreHandle, "ConfigGetParameterType");
    ConfigGetParameterHelp = (ptr_ConfigGetParameterHelp) osal_dynlib_getproc(CoreHandle, "ConfigGetParameterHelp");
    ConfigSetDefaultInt = (ptr_ConfigSetDefaultInt) osal_dynlib_getproc(CoreHandle, "ConfigSetDefaultInt");
    ConfigSetDefaultFloat = (ptr_ConfigSetDefaultFloat) osal_dynlib_getproc(CoreHandle, "ConfigSetDefaultFloat");
    ConfigSetDefaultBool = (ptr_ConfigSetDefaultBool) osal_dynlib_getproc(CoreHandle, "ConfigSetDefaultBool");
    ConfigSetDefaultString = (ptr_ConfigSetDefaultString) osal_dynlib_getproc(CoreHandle, "ConfigSetDefaultString");
    ConfigGetParamInt = (ptr_ConfigGetParamInt) osal_dynlib_getproc(CoreHandle, "ConfigGetParamInt");
    ConfigGetParamFloat = (ptr_ConfigGetParamFloat) osal_dynlib_getproc(CoreHandle, "ConfigGetParamFloat");
    ConfigGetParamBool = (ptr_ConfigGetParamBool) osal_dynlib_getproc(CoreHandle, "ConfigGetParamBool");
    ConfigGetParamString = (ptr_ConfigGetParamString) osal_dynlib_getproc(CoreHandle, "ConfigGetParamString");

    ConfigGetSharedDataFilepath = (ptr_ConfigGetSharedDataFilepath) osal_dynlib_getproc(CoreHandle, "ConfigGetSharedDataFilepath");
    ConfigGetUserConfigPath = (ptr_ConfigGetUserConfigPath) osal_dynlib_getproc(CoreHandle, "ConfigGetUserConfigPath");
    ConfigGetUserDataPath = (ptr_ConfigGetUserDataPath) osal_dynlib_getproc(CoreHandle, "ConfigGetUserDataPath");
    ConfigGetUserCachePath = (ptr_ConfigGetUserCachePath) osal_dynlib_getproc(CoreHandle, "ConfigGetUserCachePath");

    /* get function pointers to the debugger functions */
    DebugSetCallbacks = (ptr_DebugSetCallbacks) osal_dynlib_getproc(CoreHandle, "DebugSetCallbacks");
    DebugSetCoreCompare = (ptr_DebugSetCoreCompare) osal_dynlib_getproc(CoreHandle, "DebugSetCoreCompare");
    DebugSetRunState = (ptr_DebugSetRunState) osal_dynlib_getproc(CoreHandle, "DebugSetRunState");
    DebugGetState = (ptr_DebugGetState) osal_dynlib_getproc(CoreHandle, "DebugGetState");
    DebugStep = (ptr_DebugStep) osal_dynlib_getproc(CoreHandle, "DebugStep");
    DebugDecodeOp = (ptr_DebugDecodeOp) osal_dynlib_getproc(CoreHandle, "DebugDecodeOp");
    DebugMemGetRecompInfo = (ptr_DebugMemGetRecompInfo) osal_dynlib_getproc(CoreHandle, "DebugMemGetRecompInfo");
    DebugMemGetMemInfo = (ptr_DebugMemGetMemInfo) osal_dynlib_getproc(CoreHandle, "DebugMemGetMemInfo");
    DebugMemGetPointer = (ptr_DebugMemGetPointer) osal_dynlib_getproc(CoreHandle, "DebugMemGetPointer");

    DebugMemRead64 = (ptr_DebugMemRead64) osal_dynlib_getproc(CoreHandle, "DebugMemRead64");
    DebugMemRead32 = (ptr_DebugMemRead32) osal_dynlib_getproc(CoreHandle, "DebugMemRead32");
    DebugMemRead16 = (ptr_DebugMemRead16) osal_dynlib_getproc(CoreHandle, "DebugMemRead16");
    DebugMemRead8 = (ptr_DebugMemRead8) osal_dynlib_getproc(CoreHandle, "DebugMemRead8");

    DebugMemWrite64 = (ptr_DebugMemWrite64) osal_dynlib_getproc(CoreHandle, "DebugMemRead64");
    DebugMemWrite32 = (ptr_DebugMemWrite32) osal_dynlib_getproc(CoreHandle, "DebugMemRead32");
    DebugMemWrite16 = (ptr_DebugMemWrite16) osal_dynlib_getproc(CoreHandle, "DebugMemRead16");
    DebugMemWrite8 = (ptr_DebugMemWrite8) osal_dynlib_getproc(CoreHandle, "DebugMemRead8");

    DebugGetCPUDataPtr = (ptr_DebugGetCPUDataPtr) osal_dynlib_getproc(CoreHandle, "DebugGetCPUDataPtr");
    DebugBreakpointLookup = (ptr_DebugBreakpointLookup) osal_dynlib_getproc(CoreHandle, "DebugBreakpointLookup");
    DebugBreakpointCommand = (ptr_DebugBreakpointCommand) osal_dynlib_getproc(CoreHandle, "DebugBreakpointCommand");

    /* start the Mupen64Plus core library, load the configuration file */
    rval = M64ERR_INTERNAL;
    // @TODO Straighten versioning stuff
    //rval = (*CoreStartup)(0x10000, (const char*)l_ConfigDirPath, (const char*)l_DataDirPath, (void*)"Core", DebugCallback, NULL, NULL);
    rval = (*CoreStartup)(0x10000, NULL, NULL, (void*)"Core", DebugCallback, NULL, NULL);

    if (rval != M64ERR_SUCCESS)
    {
        QMessageBox::critical(this, tr("Calling CoreStartup()"),
            tr("Error starting Mupen64Plus core library."));
        DetachCoreLib();
        return rval;
    }

    /* Open configuration sections */
    rval = OpenConfigurationHandles();
    if (rval != M64ERR_SUCCESS)
    {
        QMessageBox::critical(this, tr("Loading Configuration"),
            tr("Problem while loading configuration !"));
        (*CoreShutdown)();
        DetachCoreLib();
        return rval;
    }

    return rval;
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
    if ((*CoreDoCommand)(M64CMD_ROM_OPEN, (int)romlength, buffer) != M64ERR_SUCCESS)
    {
        logLine->sprintf("Error: core failed to open ROM image.");
        logList->append(*logLine);
        (*CoreShutdown)();
        DetachCoreLib();
        return false;
    }
    return true;
}

m64p_error MainWindow::DetachCoreLib()
{
  if (CoreHandle == NULL)
      return M64ERR_INVALID_STATE;

  /* set the core function pointers to NULL */
  CoreErrorMessage = NULL;
  CoreStartup = NULL;
  CoreShutdown = NULL;
  CoreAttachPlugin = NULL;
  CoreDetachPlugin = NULL;
  CoreDoCommand = NULL;
  CoreOverrideVidExt = NULL;
  CoreAddCheat = NULL;
  CoreCheatEnabled = NULL;

  ConfigListSections = NULL;
  ConfigOpenSection = NULL;
  ConfigListParameters = NULL;
  ConfigSetParameter = NULL;
  ConfigGetParameter = NULL;
  ConfigGetParameterType = NULL;
  ConfigGetParameterHelp = NULL;
  ConfigSetDefaultInt = NULL;
  ConfigSetDefaultBool = NULL;
  ConfigSetDefaultString = NULL;
  ConfigGetParamInt = NULL;
  ConfigGetParamBool = NULL;
  ConfigGetParamString = NULL;

  ConfigGetSharedDataFilepath = NULL;
  ConfigGetUserDataPath = NULL;
  ConfigGetUserCachePath = NULL;

  DebugSetCallbacks = NULL;
  DebugSetCoreCompare = NULL;
  DebugSetRunState = NULL;
  DebugGetState = NULL;
  DebugStep = NULL;
  DebugDecodeOp = NULL;
  DebugMemGetRecompInfo = NULL;
  DebugMemGetMemInfo = NULL;
  DebugMemGetPointer = NULL;

  DebugMemRead64 = NULL;
  DebugMemRead32 = NULL;
  DebugMemRead16 = NULL;
  DebugMemRead8 = NULL;

  DebugMemWrite64 = NULL;
  DebugMemWrite32 = NULL;
  DebugMemWrite16 = NULL;
  DebugMemWrite8 = NULL;

  DebugGetCPUDataPtr = NULL;
  DebugBreakpointLookup = NULL;
  DebugBreakpointCommand = NULL;

  /* detach the shared library */
  osal_dynlib_close(CoreHandle);
  CoreHandle = NULL;

  return M64ERR_SUCCESS;
}

m64p_error MainWindow::OpenConfigurationHandles(void)
{
    m64p_error rval;

    /* Open Configuration sections for core library and console User Interface */
    rval = (*ConfigOpenSection)("Core", &l_ConfigCore);
    if (rval != M64ERR_SUCCESS)
    {
        fprintf(stderr, "Error: failed to open 'Core' configuration section\n");
        return rval;
    }

    rval = (*ConfigOpenSection)("Video-General", &l_ConfigVideo);
    if (rval != M64ERR_SUCCESS)
    {
        fprintf(stderr, "Error: failed to open 'Video-General' configuration section\n");
        return rval;
    }

    rval = (*ConfigOpenSection)("UI-Console", &l_ConfigUI);
    if (rval != M64ERR_SUCCESS)
    {
        fprintf(stderr, "Error: failed to open 'UI-Console' configuration section\n");
        return rval;
    }

    /* Set default values for my Config parameters */
    (*ConfigSetDefaultString)(l_ConfigUI, "PluginDir", OSAL_CURRENT_DIR, "Directory in which to search for plugins");
    (*ConfigSetDefaultString)(l_ConfigUI, "VideoPlugin", "m64p_video_rice" OSAL_DLL_EXTENSION, "Filename of video plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "AudioPlugin", "m64p_audio_jttl" OSAL_DLL_EXTENSION, "Filename of audio plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "InputPlugin", "m64p_input_blight" OSAL_DLL_EXTENSION, "Filename of input plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "RspPlugin", "m64p_rsp_hle" OSAL_DLL_EXTENSION, "Filename of RSP plugin");

    return M64ERR_SUCCESS;
}

m64p_error MainWindow::SaveConfigurationOptions(void)
{
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
}

/* ============================================================================= */

m64p_handle GetSectionHandle (const char* name)
{
  m64p_handle handle;
  m64p_error result = (*ConfigOpenSection)(name, &handle);
  if (result != M64ERR_SUCCESS)
    qDebug () << "GetSectionHandle: unable to get" << name << "section !";

  return handle;
}

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

m64p_plugin_type MainWindow::GetPluginType (const char* filepath)
{
    /* try to open a shared library at the given filepath */
    m64p_dynlib_handle handle;
    m64p_error rval = osal_dynlib_open(&handle, filepath);
    if (rval != M64ERR_SUCCESS)
        return (m64p_plugin_type) 0;
    /* call the GetVersion function for the plugin and check compatibility */
    ptr_PluginGetVersion PluginGetVersion =
        (ptr_PluginGetVersion) osal_dynlib_getproc(handle, "PluginGetVersion");
    if (PluginGetVersion == NULL)
    {
        //if (g_Verbose)
        fprintf(stderr, "Error: library '%s' is not a Mupen64Plus library.\n", filepath);
        osal_dynlib_close(handle);
        return (m64p_plugin_type) 0;
    }

    m64p_plugin_type PluginType = (m64p_plugin_type) 0;
    int PluginVersion = 0;
    const char *PluginName = NULL;
    (*PluginGetVersion)(&PluginType, &PluginVersion, NULL, &PluginName, NULL);

    osal_dynlib_close(handle);
    return PluginType;
}

m64p_error MainWindow::PluginLoadTry(const char *filepath, int MapIndex)
{
    /* try to open a shared library at the given filepath */
    m64p_dynlib_handle handle;
    m64p_error rval = osal_dynlib_open(&handle, filepath);
    if (rval != M64ERR_SUCCESS)
        return rval;

    /* call the GetVersion function for the plugin and check compatibility */
    ptr_PluginGetVersion PluginGetVersion = (ptr_PluginGetVersion) osal_dynlib_getproc(handle, "PluginGetVersion");
    if (PluginGetVersion == NULL)
    {
        //if (g_Verbose)
        fprintf(stderr, "Error: library '%s' is not a Mupen64Plus library.\n", filepath);
        osal_dynlib_close(handle);
        return M64ERR_INCOMPATIBLE;
    }

    m64p_plugin_type PluginType = (m64p_plugin_type) 0;
    int PluginVersion = 0;
    const char *PluginName = NULL;
    (*PluginGetVersion)(&PluginType, &PluginVersion, NULL, &PluginName, NULL);
    if (PluginType != g_PluginMap[MapIndex].type)
    {
        /* the type of this plugin doesn't match with the type that was requested by the caller */
        osal_dynlib_close(handle);
        return M64ERR_INCOMPATIBLE;
    }

    /* the front-end doesn't talk to the plugins, so we don't care about the plugin version or api version */

    /* call the plugin's initialization function and make sure it starts okay */
    ptr_PluginStartup PluginStartup = (ptr_PluginStartup) osal_dynlib_getproc(handle, "PluginStartup");
    if (PluginStartup == NULL)
    {
        fprintf(stderr, "Error: library '%s' broken.  No PluginStartup() function found.\n", filepath);
        osal_dynlib_close(handle);
        return M64ERR_INCOMPATIBLE;
    }

    rval = (*PluginStartup)(CoreHandle, g_PluginMap[MapIndex].name, DebugCallback);  /* DebugCallback is in main.c */
    if (rval != M64ERR_SUCCESS)
    {
        fprintf(stderr, "Error: %s plugin library '%s' failed to start.\n", g_PluginMap[MapIndex].name, filepath);
        osal_dynlib_close(handle);
        return rval;
    }

    /* plugin loaded successfully, so set the plugin map's members */
    g_PluginMap[MapIndex].handle = handle;
    strcpy(g_PluginMap[MapIndex].filename, filepath);
    g_PluginMap[MapIndex].libname = PluginName;
    g_PluginMap[MapIndex].libversion = PluginVersion;

    return M64ERR_SUCCESS;
}

m64p_error MainWindow::UnloadPlugin (m64p_plugin_type pType)
{
    typedef m64p_error (*ptr_PluginShutdown)(void);
    ptr_PluginShutdown PluginShutdown;
    int i = 4;

    switch (pType)
    {
      case M64PLUGIN_GFX:
        i = 0;
        break;
      case M64PLUGIN_AUDIO:
        i = 1;
        break;
      case M64PLUGIN_INPUT:
        i = 2;
        break;
      case M64PLUGIN_RSP:
        i = 3;
        break;
      default:
        qDebug () << "Problem: trying to unload unknown plugin type " << pType;
        return M64ERR_INCOMPATIBLE;
        break;
    }

    if (g_PluginMap[i].handle == NULL)
      return M64ERR_SUCCESS;
    /* call the destructor function for the plugin and release the library */
    PluginShutdown = (ptr_PluginShutdown) osal_dynlib_getproc(g_PluginMap[i].handle, "PluginShutdown");
    if (PluginShutdown != NULL)
      (*PluginShutdown)();
    osal_dynlib_close(g_PluginMap[i].handle);
    /* clear out the plugin map's members */
    g_PluginMap[i].handle = NULL;
    g_PluginMap[i].filename[0] = 0;
    g_PluginMap[i].libname = NULL;
    g_PluginMap[i].libversion = 0;

    return M64ERR_SUCCESS;
}

void MainWindow::GetConfigurationSections ()
{
  configSections.clear();
  m64p_error err;
  err = (*ConfigListSections)((void*)&configSections, SectionListCallback);
  if (err != M64ERR_SUCCESS)
    qDebug () << "GetConfigurationSections: unable to list sections !";
}

m64p_error MainWindow::GetSectionParameters (const char* sectionName)
{
  m64p_handle sHandle = GetSectionHandle (sectionName);
  m64p_error res = (*ConfigListParameters)(sHandle, sHandle, ParameterListCallback);
  if (res != M64ERR_SUCCESS)
    qDebug () << "ConfigListParameters failed, return code = " << res;

  return res;
}
