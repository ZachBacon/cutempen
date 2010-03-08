/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   CuteMupen - mainwindow.cpp                                            *
 *   CuteMupen homepage: http://cutemupen.sourceforge.net/                 *
 *   Copyright (C) 2009 Richard42                                          *
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

#include <QFileDialog>      // For directory selections
#include <QMessageBox>
#include <QFileSystemModel> // for file/rom browser
#include <QTreeView>        // for file/rom browser
#include <QRegExp>          // for resolution input validation
#include <QDesktopWidget>   // to get screen geometry
#include <QFile>

extern "C" {
#include "osal/osal_preproc.h"
#include "osal/osal_dynamiclib.h"
#include "m64p_common.h"
#include "m64p_frontend.h"
#include "m64p_config.h"
#include "m64p/core_interface.h"
void DebugCallback(void *Context, int level, const char *message);
}

#include "m64p/plugin.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString title ("CuteMupen ");
    title.append(CM_VERSION_STR);
    this->setWindowTitle(title);
    ui->lb_About_NameVersion->setText(title);

    QSettings settings ("CuteMupen", "CuteMupen");

    // For the ROM browser
    dirModel = new QDirModel();

    // Get settings for the paths. If they're empty, prompt for selection
    Mupen64Library = settings.value ("Paths/Mupen64Library", "").toString();
    chooseMupen64Library(!Mupen64Library.isEmpty());
    Mupen64PluginDir = settings.value ("Paths/Mupen64PluginDir", "").toString();
    chooseMupen64PluginDir(!Mupen64PluginDir.isEmpty());
    Mupen64DataDir = settings.value ("Paths/Mupen64DataDir", "").toString();
    chooseMupen64DataDir(!Mupen64DataDir.isEmpty());
    Mupen64ConfigDir = settings.value ("Paths/Mupen64ConfigDir", "").toString();
    chooseMupen64ConfigDir(!Mupen64ConfigDir.isEmpty());
    ROMsDir = settings.value ("Paths/ROMsDir", "").toString();
    chooseROMsDir(!ROMsDir.isEmpty());

    // Get current screen resolution/size, and add it to the combobox
    QDesktopWidget* desktop = QApplication::desktop();
    int width = desktop->screenGeometry().size().width();
    int height = desktop->screenGeometry().size().height();
    QString geom;
    geom.sprintf("%dx%d", width, height);
    ui->cb_Resolution->addItem(geom);

    // Restore fullscreen setting
    ui->cb_Fullscreen->setChecked(settings.value("Video/Fullscreen", false).toBool());

    // Restore screen size setting
    QString resToRestore = settings.value("Video/Resolution", "640x480").toString();
    int idxToRestore = ui->cb_Resolution->findText(resToRestore);
    if (idxToRestore == -1) // not in the list, let's create an item for it
      ui->cb_Resolution->addItem(resToRestore);
    idxToRestore = ui->cb_Resolution->findText(resToRestore);
    ui->cb_Resolution->setCurrentIndex(idxToRestore);

    // Restore OSD setting
    ui->cb_OSD->setChecked(settings.value("Video/OSD", true).toBool());

    ROMFile = "";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


int MainWindow::clickedROM(const QModelIndex & index)
{
  ROMFile = dirModel->filePath(index);
#if defined(Q_WS_WIN)
  ROMFile.replace(QChar('/'), QChar('\\'));
#endif
  return 0;
}

int MainWindow::clickedRun()
{
    if (ROMFile.isEmpty())
    {
        QMessageBox::warning(this, "No ROM selected",
            tr("Please select a ROM file in ""ROMs"" tab."));
        return 0;
    }

	if (initMupen64() != M64ERR_SUCCESS)
		return 1;
    ApplyConfiguration();

	// (vk) this doesn't seem to get desired result on windows...
	//const char* l_ROMFilepath = ROMFile.toStdString().c_str();
    const char* l_ROMFilepath = ROMFile.toLocal8Bit();

    /* load ROM image */
    FILE *fPtr = fopen(l_ROMFilepath, "rb");
    if (fPtr == NULL)
    {
        fprintf(stderr, "Error: couldn't open ROM file '%s' for reading.\n", l_ROMFilepath);
        (*CoreShutdown)();
        DetachCoreLib();
        return 7;
    }

    /* get the length of the ROM, allocate memory buffer, load it from disk */
    long romlength = 0;
    fseek(fPtr, 0L, SEEK_END);
    romlength = ftell(fPtr);
    fseek(fPtr, 0L, SEEK_SET);
    unsigned char *ROM_buffer = (unsigned char *) malloc(romlength);
    if (ROM_buffer == NULL)
    {
        fprintf(stderr, "Error: couldn't allocate %li-byte buffer for ROM image file '%s'.\n", romlength, l_ROMFilepath);
        fclose(fPtr);
        (*CoreShutdown)();
        DetachCoreLib();
        return 8;
    }
    else if (fread(ROM_buffer, 1, romlength, fPtr) != (unsigned)romlength)
    {
        fprintf(stderr, "Error: couldn't read %li bytes from ROM image file '%s'.\n", romlength, l_ROMFilepath);
        free(ROM_buffer);
        fclose(fPtr);
        (*CoreShutdown)();
        DetachCoreLib();
        return 9;
    }
    fclose(fPtr);

    /* Try to load the ROM image into the core */
    if ((*CoreDoCommand)(M64CMD_ROM_OPEN, (int) romlength, ROM_buffer) != M64ERR_SUCCESS)
    {
        fprintf(stderr, "Error: core failed to open ROM image file '%s'.\n", l_ROMFilepath);
        free(ROM_buffer);
        (*CoreShutdown)();
        DetachCoreLib();
        return 10;
    }
    /* the core copies the ROM image, so we can release this buffer immediately */
    free(ROM_buffer);
	printf("Plugin dir is:\n%s\n", Mupen64PluginDir.toStdString().c_str());
    /* search for and load plugins */
    m64p_error rval = PluginSearchLoad(
        l_ConfigUI, Mupen64PluginDir.toStdString().c_str());
		//l_ConfigUI, Mupen64PluginDir.toLocal8Bit());
    if (rval != M64ERR_SUCCESS)
    {
        QMessageBox::critical(this, "Plugin Search",
            tr("Error searching for plugins"));
        (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
        (*CoreShutdown)();
        DetachCoreLib();
        return 12;
    }

    /* attach plugins to core */
    int i;
    for (i = 0; i < 4; i++)
    {
        if ((*CoreAttachPlugin)(g_PluginMap[i].type, g_PluginMap[i].handle) != M64ERR_SUCCESS)
        {
            QString errorString;
            errorString.sprintf("Error attaching %s plugin !", g_PluginMap[i].name);
            QMessageBox::critical(this, "Plugins", errorString);
            (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
            (*CoreShutdown)();
            DetachCoreLib();
            return 13;
        }
    }

    /* run the game */
    (*CoreDoCommand)(M64CMD_EXECUTE, 0, NULL);

    /* detach plugins from core and unload them */
    for (i = 0; i < 4; i++)
        (*CoreDetachPlugin)(g_PluginMap[i].type);
    PluginUnload();

    /* close the ROM image */
    (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);

    /* save the configuration file again if --saveoptions was specified, to keep any updated parameters from the core/plugins */
    //if (l_SaveOptions)
    //    SaveConfigurationOptions();

    /* Shut down and release the Core library */
    (*CoreShutdown)();
    DetachCoreLib();

    return 0; // ?
}

m64p_error MainWindow::initMupen64()
{
	// Load the core lib
    CoreHandle = NULL;
    m64p_error rval = M64ERR_INTERNAL;
    rval = osal_dynlib_open(&CoreHandle, Mupen64Library.toStdString().c_str());
    if (rval != M64ERR_SUCCESS || CoreHandle == NULL)
    {
        QString res;
		res.sprintf("file: %s\nrval: %x\n", Mupen64Library.toStdString().c_str(), rval);
        QMessageBox::critical(this, "Error loading Core library", res);
		return rval;
    }

    ptr_PluginGetVersion CoreVersionFunc;
    CoreVersionFunc = (ptr_PluginGetVersion) osal_dynlib_getproc(CoreHandle, "PluginGetVersion");
    if (CoreVersionFunc == NULL)
    {
        QMessageBox::critical(this, "PluginGetVersion",
            "Couldn't find PluginGetVersion()");
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
        QMessageBox::critical(this, "Calling CoreStartup()",
            tr("Error starting Mupen64Plus core library."));
        DetachCoreLib();
        return rval;
    }

    /* Open configuration sections */
    rval = OpenConfigurationHandles();
    if (rval != M64ERR_SUCCESS)
    {
        QMessageBox::critical(this, "Loading Configuration",
            tr("Problem loading config !"));
        (*CoreShutdown)();
        DetachCoreLib();
        return rval;
    }

    return rval;
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

  //QMessageBox::information(this, "Detaching Core lib", "Success !");

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
    // @TODO Use Mupen64PluginDir for PluginDir ?
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

void MainWindow::ApplyConfiguration ()
{
  QSettings settings ("CuteMupen", "CuteMupen");

  int FullScreen;
  ui->cb_Fullscreen->isChecked() ? FullScreen = 1 : FullScreen = 0;
  (*ConfigSetParameter)(l_ConfigVideo, "Fullscreen", M64TYPE_BOOL, &FullScreen);
  settings.setValue("Video/Fullscreen", ui->cb_Fullscreen->isChecked());

  QString res = ui->cb_Resolution->currentText();
  QRegExp resRegex ("^[0-9]{3,4}x[0-9]{3,4}$");
  if (resRegex.exactMatch(res))
  {
    int xres, yres;
    xres = res.section("x", 0, 0).toInt();   // leftmost field from "x" separator
    yres = res.section("x", -1, -1).toInt(); // rightmost field from "x" separator
    (*ConfigSetParameter)(l_ConfigVideo, "ScreenWidth", M64TYPE_INT, &xres);
    (*ConfigSetParameter)(l_ConfigVideo, "ScreenHeight", M64TYPE_INT, &yres);
    settings.setValue("Video/Resolution", res);
  }
  else
    QMessageBox::warning(this, tr("Invalid resolution"),
        tr("This resolution is invalid: ") + res);

  int Osd;
  ui->cb_OSD->isChecked() ? Osd = 1 : Osd = 0;
  (*ConfigSetParameter)(l_ConfigCore, "OnScreenDisplay", M64TYPE_BOOL, &Osd);
  settings.setValue("Video/OSD", ui->cb_OSD->isChecked());
}

void DebugCallback(void *Context, int level, const char *message)
{
    if (level <= 1)
        printf("%s Error: %s\n", (const char *) Context, message);
    else if (level == 2)
        printf("%s Warning: %s\n", (const char *) Context, message);
    // @TODO Manage verbose flag
    else if (level == 3 || (level == 5))// && g_Verbose))
        printf("%s: %s\n", (const char *) Context, message);
    else if (level == 4)
        printf("%s Status: %s\n", (const char *) Context, message);
    /* ignore the verbose info for now */
}
