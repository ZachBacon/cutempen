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

#include <QSettings>
#include <QRadioButton>
#include <QMessageBox>
#include <QDesktopWidget>   // to get screen geometry

extern "C" {
#include "m64p/core_interface.h"
#include "osal/osal_preproc.h"   // for shared lib extension
}
extern bool doLog;
extern bool doLogVerbose;

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
    (*ConfigSetParameter)(l_ConfigVideo, "ScreenWidth", M64TYPE_INT, &xres);
    (*ConfigSetParameter)(l_ConfigVideo, "ScreenHeight", M64TYPE_INT, &yres);
    settings.setValue("Video/Resolution", res);
}
  else
    QMessageBox::warning(this, tr("Invalid resolution"),
        tr("This resolution is invalid: ") + res);

}


void MainWindow::RestoreSettings ()
{
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

  // Find all graphics plugins and fill combo-box
  QDir pluginDir (Mupen64PluginDir);
  pluginDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
  QStringList filters;
  filters << "*" OSAL_DLL_EXTENSION;
  pluginDir.setNameFilters(filters);
  for (int i = 0; i < pluginDir.entryList().count(); i++)
  {
      m64p_plugin_type pType = GetPluginType (
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



  // Restore EmuMode setting
  int emuMode = settings.value("Settings/EmuMode", 2).toInt();
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
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");

  int FullScreen;
  ui->cb_Fullscreen->isChecked() ? FullScreen = 1 : FullScreen = 0;
  (*ConfigSetParameter)(l_ConfigVideo, "Fullscreen", M64TYPE_BOOL, &FullScreen);
  //settings.setValue("Video/Fullscreen", ui->cb_Fullscreen->isChecked());

  int Osd;
  ui->cb_OSD->isChecked() ? Osd = 1 : Osd = 0;
  (*ConfigSetParameter)(l_ConfigCore, "OnScreenDisplay", M64TYPE_BOOL, &Osd);
  //settings.setValue("Video/OSD", ui->cb_OSD->isChecked());

  int emumode;
  emumode = settings.value("Settings/EmuMode", "2").toInt();
  if ((emumode >= 0) && (emumode <= 2))
    (*ConfigSetParameter)(l_ConfigCore, "R4300Emulator", M64TYPE_INT, &emumode);
}

void MainWindow::chooseGfxPlugin (QString text)
{
    (*ConfigSetParameter)(l_ConfigUI, "VideoPlugin", M64TYPE_STRING,
                          text.toLocal8Bit().constData());
}
