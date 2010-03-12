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

#include <m64p/core_interface.h>

void MainWindow::toggledEmuMode(bool /*checked*/)
{
  QSettings settings ("CuteMupen", "CuteMupen");
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

  int emumode;
  emumode = settings.value("Settings/EmuMode", "2").toInt();
  if ((emumode >= 0) && (emumode <= 2))
    (*ConfigSetParameter)(l_ConfigCore, "R4300Emulator", M64TYPE_INT, &emumode);
}
