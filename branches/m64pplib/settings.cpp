/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   CuteMupen - settings.cpp                                              *
 *   CuteMupen homepage: http://cutemupen.sourceforge.net/                 *
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

#include "plugindialog.h"
#include "ui_plugindialog.h"
#include "inputdialog.h"

#include <QDebug>
#include <QSettings>
#include <QRadioButton>
#include <QMessageBox>
#include <QDesktopWidget>   // to get screen geometry
#include <QChar>

#include "mupen64plusplus/osal_preproc.h"
#include "mupen64plusplus/MupenAPI.h"
#include "mupen64plusplus/MupenAPIpp.h"
#include "mupen64plusplus/plugin.h"
extern ptr_ConfigSetParameter PtrConfigSetParameter;

extern bool doLog;
extern bool doLogVerbose;
//extern QStringList parameterList;
extern PluginDialog* pDialog;
extern InputDialog* inputDialog;
extern m64p_handle GetSectionHandle (const char* name);

extern m64p_plugin_type GetPluginType (const char* filepath);

void MainWindow::toggledEmuMode(bool /*checked*/)
{
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");
  if (ui->rb_PureInterpreter->isChecked())
  {
    settings.setValue("Settings/EmuMode", 0);
  }
  else if (ui->rb_Interpreter->isChecked())
  {
    settings.setValue("Settings/EmuMode", 1);
  }
  else if (ui->rb_DynaRec->isChecked())
  {
    settings.setValue("Settings/EmuMode", 2);
  }
}

void MainWindow::toggledLogging(bool /*checked*/)
{
  if (ui->rb_Log_Disabled->isChecked())
  {
    doLog = false;
    doLogVerbose = false;
  }
  else if (ui->rb_Log_Enabled->isChecked())
  {
    doLog = true;
    doLogVerbose = false;
  }
  else if (ui->rb_Log_Verbose->isChecked())
  {
    doLog = true;
    doLogVerbose = true;
  }
}

void MainWindow::toggledOSD(bool checked)
{
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");
  settings.setValue("Video/OSD", checked);
}

void MainWindow::toggledFullscreen(bool checked)
{
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");
  settings.setValue("Video/Fullscreen", checked);
}

void MainWindow::chooseResolution (QString /*text*/)
{
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");
  QString res = ui->cb_Resolution->currentText();
  QRegExp resRegex ("^[0-9]{3,4}x[0-9]{3,4}$");
  if (resRegex.exactMatch(res))
  {
    int xres, yres;
    xres = res.section("x", 0, 0).toInt();   // leftmost field from "x" separator
    yres = res.section("x", -1, -1).toInt(); // rightmost field from "x" separator
    (*PtrConfigSetParameter)(l_ConfigVideo, "ScreenWidth", M64TYPE_INT, &xres);
    (*PtrConfigSetParameter)(l_ConfigVideo, "ScreenHeight", M64TYPE_INT, &yres);
    settings.setValue("Video/Resolution", res);
  }
  else
    QMessageBox::warning(this, tr("Invalid resolution"),
        tr("This resolution is invalid: ") + res);
}


void MainWindow::RestoreSettings ()
{
#if 0
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");
  // Get settings for the paths. If they're empty, prompt for selection
  Mupen64PluginDir = settings.value ("Paths/Mupen64PluginDir", "").toString();
  chooseMupen64PluginDir(!Mupen64PluginDir.isEmpty());
  Mupen64Library = settings.value ("Paths/Mupen64Library", "").toString();
  chooseMupen64Library(!Mupen64Library.isEmpty());
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
#endif

  l_ConfigUI = ::getConfigUI();
  char M64PluginDir[1024];
  if (getStringConfigParam(l_ConfigUI, "PluginDir", M64PluginDir, 1000)
          != M64ERR_SUCCESS)
      printf("Problem getting PluginDir from conf !!!\n");(stdout);
  Mupen64PluginDir = QString::fromAscii(M64PluginDir);
  QString Library = Mupen64PluginDir;
  ui->le_PluginsDir->setText(Library);
  Library.append(OSAL_DIR_SEPARATOR);
  Library.append(OSAL_DEFAULT_DYNLIB_FILENAME);
  setStringConfigParam(l_ConfigUI, "Library",
                       Library.toLocal8Bit().constData());
  ui->le_Library->setText(Library);

  // Get selected plugins from configuration settings before they're overwritten by discovery
  char GfxPlugin[1024];// = settings.value("Settings/GfxPlugin").toString();
  char SndPlugin[1024];// = settings.value("Settings/SndPlugin").toString();
  char InpPlugin[1024];// = settings.value("Settings/InpPlugin").toString();
  char RspPlugin[1024];// = settings.value("Settings/RspPlugin").toString();
  if (getStringConfigParam(l_ConfigUI, "VideoPlugin", GfxPlugin, 1024) != M64ERR_SUCCESS)
      printf("Problem getting VideoPlugin from conf !!!\n");
  if (getStringConfigParam(l_ConfigUI, "AudioPlugin", SndPlugin, 1024) != M64ERR_SUCCESS)
      printf("Problem getting AudioPlugin from conf !!!\n");
  if (getStringConfigParam(l_ConfigUI, "InputPlugin", InpPlugin, 1024) != M64ERR_SUCCESS)
      printf("Problem getting InputPlugin from conf !!!\n");
  if (getStringConfigParam(l_ConfigUI, "RspPlugin", RspPlugin, 1024) != M64ERR_SUCCESS)
      printf("Problem getting RspPlugin from conf !!!\n");
  fflush(stdout);

  // Find all plugins and fill combo-boxes
  QDir pluginDir (Mupen64PluginDir);
  pluginDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
  QStringList filters;
  filters << "*" OSAL_DLL_EXTENSION;
  pluginDir.setNameFilters(filters);
  for (int i = 0; i < pluginDir.entryList().count(); i++)
  {
    m64p_plugin_type pType = ::GetPluginType (
        pluginDir.absoluteFilePath(pluginDir[i]).toLocal8Bit().constData());
    switch (pType)
    {
      case M64PLUGIN_GFX:
        ui->cb_GfxPlugin->addItem (pluginDir[i]);
        break;
      case M64PLUGIN_AUDIO:
        ui->cb_SndPlugin->addItem (pluginDir[i]);
        break;
      case M64PLUGIN_INPUT:
        ui->cb_InpPlugin->addItem (pluginDir[i]);
        break;
      case M64PLUGIN_RSP:
        ui->cb_RspPlugin->addItem (pluginDir[i]);
        break;
      default:
        break;
    }
  }

  int idx;
  idx = ui->cb_GfxPlugin->findText (GfxPlugin);
  if (idx != -1)
    ui->cb_GfxPlugin->setCurrentIndex (idx);
  idx = ui->cb_SndPlugin->findText (SndPlugin);
  if (idx != -1)
    ui->cb_SndPlugin->setCurrentIndex (idx);
  idx = ui->cb_InpPlugin->findText (InpPlugin);
  if (idx != -1)
    ui->cb_InpPlugin->setCurrentIndex (idx);
  idx = ui->cb_RspPlugin->findText (RspPlugin);
  if (idx != -1)
    ui->cb_RspPlugin->setCurrentIndex (idx);

  //Test (); // Check if we can get some plugin parameters

  // Restore EmuMode setting
  //int emuMode = settings.value("Settings/EmuMode", 2).toInt();
  int emuMode = 2;
  switch (emuMode)
  {
    case 2:
      ui->rb_DynaRec->setChecked(true);
      break;
    case 1:
      ui->rb_Interpreter->setChecked(true);
      break;
    case 0:
      ui->rb_PureInterpreter->setChecked(true);
      break;
    default:
      QMessageBox::warning(this, tr("Unknown EmuMode setting"),
          tr("Invalid value for EmuMode"));
  }
}

void MainWindow::ApplyConfiguration ()
{
#if 0
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");

  int FullScreen;
  ui->cb_Fullscreen->isChecked() ? FullScreen = 1 : FullScreen = 0;
  (*PtrConfigSetParameter)(l_ConfigVideo, "Fullscreen", M64TYPE_BOOL, &FullScreen);
  //settings.setValue("Video/Fullscreen", ui->cb_Fullscreen->isChecked());

  int Osd;
  ui->cb_OSD->isChecked() ? Osd = 1 : Osd = 0;
  (*PtrConfigSetParameter)(l_ConfigCore, "OnScreenDisplay", M64TYPE_BOOL, &Osd);
  //settings.setValue("Video/OSD", ui->cb_OSD->isChecked());

  int emumode;
  emumode = settings.value("Settings/EmuMode", "2").toInt();
  if ((emumode >= 0) && (emumode <= 2))
    (*PtrConfigSetParameter)(l_ConfigCore, "R4300Emulator", M64TYPE_INT, &emumode);
#endif
}

void MainWindow::chooseGfxPlugin (QString text)
{
    GetConfigurationSections();
    printf("chooseGfxPlugin (%s)\n\t%d sections\n", text.toLocal8Bit().constData(), configSections.size());
    fflush(stdout);
    for (int idx = 0; idx < configSections.size(); idx++)
    {
        if (!strcmp(configSections[idx].m_section_name.c_str(), "UI-CuteMupen")
                && (configSections[idx].hasChildNamed("VideoPlugin")))
        {
            printf("chooseGfxPlugin : idx = %d for section %s\n", idx, configSections[idx].m_section_name.c_str());
            ConfigSection cfggfx = configSections[idx];
            cfggfx.getParamWithName("VideoPlugin")->setStringValue(text.toStdString());
            saveConfig();
            printf("UI-CuteMupen: set VideoPlugin = %s\n", text.toLocal8Bit().constData());
            fflush(stdout);
            //m_api->loadPlugins();
            QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;
            //::PluginLoadTry (filePath.toLocal8Bit().constData(), M64PLUGIN_GFX);
            ::attachPlugins();
        }
    }

#if 0
    (*PtrConfigSetParameter)(l_ConfigUI, "VideoPlugin", M64TYPE_STRING,
                          text.toLocal8Bit().constData());
    QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;
    if (ActivatePlugin (filePath.toLocal8Bit().constData(), M64PLUGIN_GFX) != M64ERR_SUCCESS)
    {
      qDebug () << "chooseGfxPlugin failed !";
      return;
    }
    //QSettings settings ("CuteMupen", "CuteMupen");
    //settings.setIniCodec("UTF-8");
    //settings.setValue("Settings/GfxPlugin", text.toLocal8Bit().constData());
#endif
}

void MainWindow::chooseSndPlugin (QString text)
{
#if 0
    (*PtrConfigSetParameter)(l_ConfigUI, "AudioPlugin", M64TYPE_STRING,
                          text.toLocal8Bit().constData());
    QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;
    if (ActivatePlugin (filePath.toLocal8Bit().constData(), M64PLUGIN_AUDIO) != M64ERR_SUCCESS)
    {
      qDebug () << "chooseSndPlugin failed !";
      return;
    }
    QSettings settings ("CuteMupen", "CuteMupen");
    settings.setIniCodec("UTF-8");
    settings.setValue("Settings/SndPlugin", text.toLocal8Bit().constData());
#endif
}

void MainWindow::chooseInpPlugin (QString text)
{
#if 0
  (*PtrConfigSetParameter)(l_ConfigUI, "InputPlugin", M64TYPE_STRING,
                        text.toLocal8Bit().constData());
  QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;
  if (ActivatePlugin (filePath.toLocal8Bit().constData(), M64PLUGIN_INPUT) != M64ERR_SUCCESS)
  {
    qDebug () << "chooseInpPlugin failed !";
    return;
  }
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");
  settings.setValue("Settings/InpPlugin", text.toLocal8Bit().constData());
#endif
}

void MainWindow::chooseRspPlugin (QString text)
{
#if 0
  (*PtrConfigSetParameter)(l_ConfigUI, "RspPlugin", M64TYPE_STRING,
                        text.toLocal8Bit().constData());
  QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;
  if (ActivatePlugin (filePath.toLocal8Bit().constData(), M64PLUGIN_RSP) != M64ERR_SUCCESS)
  {
    qDebug () << "chooseRspPlugin failed !";
    return;
  }
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");
  settings.setValue("Settings/RspPlugin", text.toLocal8Bit().constData());
#endif
}


void MainWindow::clickedGfx ()
{
  GetConfigurationSections ();
  // Extract plugin name from plugin file name
  QString pluginName = ui->cb_GfxPlugin->currentText();
  pluginName = pluginName.mid(pluginName.lastIndexOf('-') + 1);
  // Suppress the filename extension
  pluginName.chop(pluginName.length() - pluginName.lastIndexOf('.'));
  pluginName[0] = pluginName[0].toUpper();
  QString sectionName;
  sectionName.sprintf("Video-%s", pluginName.toLocal8Bit().constData());
  pDialog = NULL;
  //QStringList subset = configSections.filter(sectionName, Qt::CaseInsensitive);
  //ConfigSection cfgVideo = configSections[0];
#if 0
  if (subset.count() == 1)
  {
      pDialog = new PluginDialog (this,
          GetSectionHandle (sectionName.toLocal8Bit().constData()),
          sectionName.toLocal8Bit().constData());
      GetSectionParameters (sectionName.toLocal8Bit().constData());
      pDialog->exec();
  }
  else
  {
    qDebug () << "Not sure what to do, found " << subset.count () << " matching elements";
    QMessageBox::information (this, "Nothing to configure",
        "Couldn't find parameters in " + sectionName + " section.");
    return;
  }
#endif
}

void MainWindow::clickedSnd ()
{
  GetConfigurationSections ();
  // Extract plugin name from plugin file name
  QString pluginName = ui->cb_SndPlugin->currentText();
  pluginName = pluginName.mid(pluginName.lastIndexOf('-') + 1);
  // Suppress the filename extension
  pluginName.chop(pluginName.length() - pluginName.lastIndexOf('.'));
  pluginName[0] = pluginName[0].toUpper();
  QString sectionName;
  sectionName.sprintf("Audio-%s", pluginName.toLocal8Bit().constData());
  pDialog = NULL;
#if 0
  QStringList subset = configSections.filter(sectionName, Qt::CaseInsensitive);
  if (subset.count() == 1)
  {
      pDialog = new PluginDialog (this,
          GetSectionHandle (sectionName.toLocal8Bit().constData()),
          sectionName.toLocal8Bit().constData());
      GetSectionParameters (sectionName.toLocal8Bit().constData());
      pDialog->exec();
  }
  else
  {
    qDebug () << "Not sure what to do, found " << subset.count () << " matching elements";
    for (int i = 0; i < subset.count(); i++)
      qDebug () << subset[i];
    QMessageBox::information (this, "Nothing to configure",
        "Couldn't find parameters in " + sectionName + " section.");
    return;
  }
#endif
}

void MainWindow::clickedInp ()
{
#if 0
  GetConfigurationSections ();
  // Delete any section not finishing with a number, since Input-Sdl-Control
  // gets created on query...
  for (int i = 0; i < configSections.count(); i++)
  {
      QChar ch = configSections[i].at(configSections[i].length() -1);
      if (ch.isLetter ())
          configSections.removeAt(i);
  }
  // Extract plugin name from plugin file name
  QString pluginName = ui->cb_InpPlugin->currentText();
  pluginName = pluginName.mid(pluginName.lastIndexOf('-') + 1);
  // Suppress the filename extension
  pluginName.chop(pluginName.length() - pluginName.lastIndexOf('.'));
  pluginName[0] = pluginName[0].toUpper();
  QString sectionName;
  sectionName.sprintf("Input-%s-Control", pluginName.toLocal8Bit().constData());
  inputDialog = NULL;
  QStringList subset = configSections.filter(sectionName, Qt::CaseInsensitive);
  subset.sort();
  if (subset.count() == 4)
  {
      m64p_handle cfgHandle[4];
      for (int i = 0; i< 4; i++)
      {
        cfgHandle[i] = GetSectionHandle (subset[i].toLocal8Bit().constData());
      }

      inputDialog = new InputDialog (this, cfgHandle, subset);

      for (int i = 0; i < 4; i++)
      {
        //qDebug () << "Getting section parameters for" << subset[i].toLocal8Bit().constData();
        GetSectionParameters (subset[i].toLocal8Bit().constData());
        if (i < 3)
          inputDialog->NextTab ();
      }
      inputDialog->SetCurrentTab (0);
      inputDialog->exec();
      delete inputDialog;
  }
  else
  {
    qDebug () << "Found an unexpected number (" << subset.count () << ") of matching elements:";
    for (int i = 0; i < subset.count(); i++)
      qDebug () << subset[i];
    QMessageBox::information (this, "Nothing to configure",
        "Couldn't find parameters for " + sectionName + " section.");
  }
#endif
}

void MainWindow::clickedRsp ()
{
#if 0
  GetConfigurationSections ();
  // Extract plugin name from plugin file name
  QString pluginName = ui->cb_RspPlugin->currentText();
  pluginName = pluginName.mid(pluginName.lastIndexOf('-') + 1);
  // Suppress the filename extension
  pluginName.chop(pluginName.length() - pluginName.lastIndexOf('.'));
  pluginName[0] = pluginName[0].toUpper();
  QString sectionName;
  sectionName.sprintf("Rsp-%s", pluginName.toLocal8Bit().constData());
  pDialog = NULL;
  QStringList subset = configSections.filter(pluginName, Qt::CaseInsensitive);
  if (subset.count() == 1)
  {
      pDialog = new PluginDialog (this,
          GetSectionHandle (sectionName.toLocal8Bit().constData()),
          sectionName.toLocal8Bit().constData());
      GetSectionParameters (sectionName.toLocal8Bit().constData());
      pDialog->exec();
  }
  else
  {
    qDebug () << "Not sure what to do, found " << subset.count () << " matching elements";
    QMessageBox::information (this, "Nothing to configure",
        "Couldn't find parameters in " + sectionName + " section.");
    return;
  }
#endif
}
