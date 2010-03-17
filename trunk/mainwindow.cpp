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

// For log text
QString* logLine;
QStringList* logList;
bool doLog;
bool doLogVerbose;

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

    QString title ("CuteMupen ");
    title.append(CM_VERSION_STR);
    this->setWindowTitle(title);
    ui->lb_About_NameVersion->setText(title);

    // For the ROM browser
    dirModel = new QDirModel();
    ROMFile = "";

    RestoreSettings();
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
  return 0;
}

int MainWindow::clickedRun()
{
    ui->te_Logs->clear();
    if (!isCoreReady)
    {
        QMessageBox::warning(this, tr("Core library not ready"),
            tr("Did you select M64+ library file in Paths tab ?"));
        return 1;
    }

    ApplyConfiguration();

    if (ROMFile.isEmpty())
    {
        QMessageBox::warning(this, tr("No ROM selected"),
            tr("Please select a ROM file."));
        ui->tab_Paths->setCurrentIndex(2);
        return 1;
    }

    if (!LoadRom(ROMFile))
    {
        QMessageBox::critical(this, tr("Error loading ROM"),
            tr("Unable to load ROM !"));
        return 2;
    }

    /* search for and load plugins */
    m64p_error rval = PluginSearchLoad(
        l_ConfigUI, Mupen64PluginDir.toLocal8Bit().constData());
    if (rval != M64ERR_SUCCESS)
    {
        QMessageBox::critical(this, tr("Plugin Search"),
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

    // Display log in Logs tab
    for (i = 0; i < logList->size(); i++)
        ui->te_Logs->append(logList->at(i));
    logList->clear();
    return 0;
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
