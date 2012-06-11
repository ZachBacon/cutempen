/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   CuteMupen - mainwindow.cpp                                            *
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

#include <QDebug>
#include <QFileDialog>      // For directory selections
#include <QMessageBox>
#include <QFileSystemModel> // for file/rom browser
#include <QTreeView>        // for file/rom browser
#include <QRegExp>          // for resolution input validation
#include <QFile>

#include "mupen64plusplus/MupenAPIpp.h"

// For log text
QString* logLine;
QStringList* logList;
bool doLog;
bool doLogVerbose;

QStringList parameterList;
PluginDialog* pDialog;
InputDialog* inputDialog;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isCoreReady = false;
    doLog = true;
    doLogVerbose = false;
    logLine = new QString();
    logList = new QStringList();
    pDialog = NULL;
    inputDialog = NULL;

    QString title ("CuteMupen");
    this->setWindowTitle(title);
    title.append(" " CM_VERSION_STR);
    ui->lb_About_NameVersion->setText(title);

    // For the ROM browser
    dirModel = new QDirModel();
    ROMFile = "";

    //deplacé dans InitMupen64()
    //RestoreSettings();

}

MainWindow::~MainWindow()
{
  if (isCoreReady)
  {
    /* Shut down and release the Core library */
    (*CoreShutdown)();
    DetachCoreLib();
  }
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
  Mupen64PlusPlus::RomInfo info = m_api->getRomInfo(ROMFile);
  ui->statusBar->showMessage(info.country);
  return 0;
}

int MainWindow::clickedRun()
{
    // Let's disable "Run" button to prevent multiple launches
    ui->pb_Run->setDisabled(true);
    ui->te_Logs->clear();
    if (!isCoreReady)
    {
        QMessageBox::warning(this, tr("Core library not ready"),
            tr("Did you select M64+ library file in Paths tab ?"));
        ui->pb_Run->setDisabled(false);
        return 1;
    }

    ApplyConfiguration();

    if (ROMFile.isEmpty())
    {
        QMessageBox::warning(this, tr("No ROM selected"),
            tr("Please select a ROM file."));
        ui->tab_Paths->setCurrentIndex(2);
        ui->pb_Run->setDisabled(false);
        return 1;
    }

    LoadFile(ROMFile);
    m_api->runEmulation();
    m_api->closeRom();
    /* save the configuration file again if --saveoptions was specified, to keep any updated parameters from the core/plugins */
    //if (l_SaveOptions)
    //    SaveConfigurationOptions();

    FlushLog ();

    ui->pb_Run->setDisabled(false);
    return 0;
}

void MainWindow::FlushLog ()
{
  // Display log in Logs tab
  for (int i = 0; i < logList->size(); i++)
      ui->te_Logs->append(logList->at(i));
  logList->clear();
}

void DebugCallback(void *Context, int level, const char *message)
{
    QString line;
    if (!doLog)
      return;
    if (level <= 1)
        line.sprintf("%s Error: %s", (const char *) Context, message);
    else if (level == 2)
        line.sprintf("%s Warning: %s", (const char *) Context, message);
    else if (level == 3 || ((level == 5) && doLogVerbose))
        line.sprintf("%s: %s", (const char *) Context, message);
    else if (level == 4)
        line.sprintf("%s Status: %s", (const char *) Context, message);
    if (!line.isEmpty())
        logList->append(line);
}
