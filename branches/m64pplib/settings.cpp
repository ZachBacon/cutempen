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

extern m64p_plugin_type GetPluginType (const char* filepath);

void MainWindow::toggledEmuMode(bool /*checked*/)
{
  ConfigSection* cfg = GetSection ("Core");
  if (!cfg)
      return;
  ConfigParam* par = cfg->getParamWithName("R4300Emulator");
  if (!par)
      return;

  if (ui->rb_PureInterpreter->isChecked())
  {
    par->setIntValue(0);
  }
  else if (ui->rb_Interpreter->isChecked())
  {
    par->setIntValue(1);
  }
  else if (ui->rb_DynaRec->isChecked())
  {
    par->setIntValue(2);
  }
  saveConfig();
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
  ConfigSection* cfg = GetSection("Core");
  if (!cfg)
      return;
  cfg->getParamWithName("OnScreenDisplay")->setBoolValue(checked ? 1 : 0);
  saveConfig();
}

void MainWindow::toggledFullscreen(bool checked)
{
  ConfigSection* cfg = GetSection("Video-General");
  if (!cfg)
      return;
  cfg->getParamWithName("Fullscreen")->setBoolValue(checked ? 1 : 0);
    saveConfig();
}

void MainWindow::chooseResolution (QString /*text*/)
{
  ConfigSection* cfg = GetSection("Video-General");
  if (!cfg)
      return;

  QString res = ui->cb_Resolution->currentText();
  QRegExp resRegex ("^[0-9]{3,4}x[0-9]{3,4}$");
  if (resRegex.exactMatch(res))
  {
    int xres, yres;
    xres = res.section("x", 0, 0).toInt();   // leftmost field from "x" separator
    yres = res.section("x", -1, -1).toInt(); // rightmost field from "x" separator
    cfg->getParamWithName("ScreenWidth")->setIntValue(xres);
    cfg->getParamWithName("ScreenHeight")->setIntValue(yres);
    saveConfig();
  }
  else
    QMessageBox::warning(this, tr("Invalid resolution"),
        tr("This resolution is invalid: ") + res);
}


void MainWindow::RestoreSettings ()
{
  // Get current screen resolution/size, and add it to the combobox
  QDesktopWidget* desktop = QApplication::desktop();
  int width = desktop->screenGeometry().size().width();
  int height = desktop->screenGeometry().size().height();
  QString geom;
  geom.sprintf("%dx%d", width, height);
  ui->cb_Resolution->addItem(geom);
  // Now add the one from config
  ConfigSection* cfg_videogen = GetSection("Video-General");
  if (!cfg_videogen)
      return;
  QString res;
  res.sprintf("%dx%d",
              cfg_videogen->getParamWithName("ScreenWidth")->getIntValue(),
              cfg_videogen->getParamWithName("ScreenHeight")->getIntValue());
  // make sure it's in combobox, and select it
  int idxToRestore = ui->cb_Resolution->findText(res);
  if (idxToRestore == -1) // not in the list, let's create an item for it
    ui->cb_Resolution->addItem(res);
  idxToRestore = ui->cb_Resolution->findText(res);
  ui->cb_Resolution->setCurrentIndex(idxToRestore);

  // Restore fullscreen setting
  ui->cb_Fullscreen->setChecked(cfg_videogen->getParamWithName("Fullscreen")->getBoolValue());

  // Restore OSD setting
  ConfigSection* cfg_core = GetSection("Core");
  if (!cfg_core)
      return;
  ui->cb_OSD->setChecked(cfg_core->getParamWithName("OnScreenDisplay")->getBoolValue());

  // Restore EmuMode setting
  int emuMode = cfg_core->getParamWithName("R4300Emulator")->getIntValue();
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

  ConfigSection* cfg_ui = GetSection("UI-CuteMupen");
  if (!cfg_ui)
      return;

  // Restore the path to ROMs directory
  ROMsDir = QString::fromStdString(
              cfg_ui->getParamWithName("GamesPath")->getStringValue());
  UpdateROMsDir();

  // Get selected plugins from configuration settings before they're overwritten by discovery
  Mupen64PluginDir = QString::fromStdString(
              cfg_ui->getParamWithName("PluginDir")->getStringValue());
  QString GfxPlugin = QString::fromStdString(
              cfg_ui->getParamWithName("VideoPlugin")->getStringValue());
  QString SndPlugin = QString::fromStdString(
              cfg_ui->getParamWithName("AudioPlugin")->getStringValue());
  QString InpPlugin = QString::fromStdString(
              cfg_ui->getParamWithName("InputPlugin")->getStringValue());
  QString RspPlugin = QString::fromStdString(
              cfg_ui->getParamWithName("RspPlugin")->getStringValue());

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
  else
      ui->cb_GfxPlugin->setCurrentIndex(0);
  idx = ui->cb_SndPlugin->findText (SndPlugin);
  if (idx != -1)
    ui->cb_SndPlugin->setCurrentIndex (idx);
  idx = ui->cb_InpPlugin->findText (InpPlugin);
  if (idx != -1)
    ui->cb_InpPlugin->setCurrentIndex (idx);
  idx = ui->cb_RspPlugin->findText (RspPlugin);
  if (idx != -1)
    ui->cb_RspPlugin->setCurrentIndex (idx);
}

void MainWindow::chooseGfxPlugin (QString text)
{
    ConfigSection* cfg = GetSection("UI-CuteMupen");
    if (!cfg)
        return;
    cfg->getParamWithName("VideoPlugin")->setStringValue(text.toStdString());
    saveConfig();

    QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;

    if (ActivatePlugin (filePath.toLocal8Bit().constData(), M64PLUGIN_GFX) != M64ERR_SUCCESS)
    //if (PluginLoadTry (filePath.toLocal8Bit().constData(), M64PLUGIN_GFX) != M64ERR_SUCCESS)
      qDebug () << "chooseGfxPlugin failed !";
}

void MainWindow::chooseSndPlugin (QString text)
{
    ConfigSection* cfg = GetSection("UI-CuteMupen");
    if (!cfg)
        return;
    cfg->getParamWithName("AudioPlugin")->setStringValue(text.toStdString());
    saveConfig();

    QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;

    if (ActivatePlugin (filePath.toLocal8Bit().constData(), M64PLUGIN_AUDIO) != M64ERR_SUCCESS)
      qDebug () << "chooseSndPlugin failed !";
}

void MainWindow::chooseInpPlugin (QString text)
{
    ConfigSection* cfg = GetSection("UI-CuteMupen");
    if (!cfg)
        return;
    cfg->getParamWithName("InputPlugin")->setStringValue(text.toStdString());
    saveConfig();

    QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;

    if (ActivatePlugin (filePath.toLocal8Bit().constData(), M64PLUGIN_INPUT) != M64ERR_SUCCESS)
      qDebug () << "chooseInpPlugin failed !";
}

void MainWindow::chooseRspPlugin (QString text)
{
    ConfigSection* cfg = GetSection("UI-CuteMupen");
    if (!cfg)
        return;
    cfg->getParamWithName("RspPlugin")->setStringValue(text.toStdString());
    saveConfig();

    QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;

    if (ActivatePlugin (filePath.toLocal8Bit().constData(), M64PLUGIN_RSP) != M64ERR_SUCCESS)
      qDebug () << "chooseRspPlugin failed !";
}


void MainWindow::clickedGfx ()
{
  // Extract plugin name from plugin file name
  QString pluginName = ui->cb_GfxPlugin->currentText();
  pluginName = pluginName.mid(pluginName.lastIndexOf('-') + 1);
  // Suppress the filename extension
  pluginName.chop(pluginName.length() - pluginName.lastIndexOf('.'));
  pluginName[0] = pluginName[0].toUpper();
  QString sectionName;
  sectionName.sprintf("Video-%s", pluginName.toLocal8Bit().constData());

  ConfigSection* cfg = GetSection(sectionName.toLocal8Bit().constData());
  if (!cfg)
      return;

  //pDialog = NULL;
  //QStringList subset = configSections.filter(sectionName, Qt::CaseInsensitive);
  //ConfigSection cfgVideo = configSections[0];

  pDialog = new PluginDialog (this, cfg->m_handle,
                              QString::fromStdString(cfg->m_section_name).toLocal8Bit().constData());
  pDialog->exec();

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
  }    ConfigSection* cfg = GetSection("UI-CuteMupen");
  if (!cfg)
      return;
  cfg->getParamWithName("VideoPlugin")->setStringValue(text.toStdString());
  saveConfig();

  QString filePath = Mupen64PluginDir + OSAL_DIR_SEPARATOR + text;

  if (ActivatePlugin (filePath.toLocal8Bit().constData(), M64PLUGIN_GFX) != M64ERR_SUCCESS)
    qDebug () << "chooseGfxPlugin failed !";
  else
  {
    qDebug () << "Not sure what to do, found " << subset.count () << " matching elements";
    QMessageBox::information (this, "Nothing to configure",
        "Couldn't find parameters in " + sectionName + " section.");
    return;
  }
#endif
}
