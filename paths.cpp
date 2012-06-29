/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   CuteMupen - paths.cpp                                                 *
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

#include <QFileDialog>         // For directory selections
#include <QFileSystemModel>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

#include "mupen64plusplus/osal_preproc.h" // For default core library filename

void MainWindow::chooseMupen64Library(bool skipDialog)
{
    if (!skipDialog)
    {
        QString filter ("Library (");
        filter.append(OSAL_DEFAULT_DYNLIB_FILENAME);
        filter.append(")");
        Mupen64Library = QFileDialog::getOpenFileName(this,
            tr("Select Mupen64 core library file"), QDir::currentPath(), filter);
    }
    UpdateM64Library();
}

void MainWindow::editedMupen64Library()
{
  QString path;
  path = ui->lb_Library_Path->text();
  if (!QFile::exists(path))
  {
    QMessageBox::warning(this, tr("File does not exist"),
        tr("Please set the library file to a valid file."));
    ui->lb_Library_Path->setText(QDir::currentPath());
    return;
  }
  Mupen64Library = path;
  UpdateM64Library();
}

void MainWindow::UpdateM64Library()
{
  if (isCoreReady)
  {
    // Shut down and release the Core library
    ::CoreShutdown();
    ::DetachCoreLib();
    isCoreReady = false;
  }
  if (!Mupen64Library.isEmpty())
  {
    // Save the chosen directory in CuteMupen config
    QSettings settings ("CuteMupen", "CuteMupen");
    settings.setIniCodec("UTF-8");
    settings.setValue("Paths/Mupen64Library", Mupen64Library);
    // Set line edit to the chosen directory
    //ui->le_Library->setText(Mupen64Library);
    ui->lb_Library_Path->setText(
                Mupen64Library.mid(Mupen64Library.lastIndexOf(OSAL_DIR_SEPARATOR) + 1));

#if defined(Q_WS_WIN)
    {
      Mupen64Library.replace("/", "\\");
    }
#endif
  }
  else
      return;

  if (InitMupen64() != M64ERR_SUCCESS)
  {
    isCoreReady = false;
    return;
  }
  isCoreReady = true;
}

void MainWindow::chooseMupen64PluginDir(bool skipDialog)
{
  if (!skipDialog)
  {
    Mupen64PluginDir = QFileDialog::getExistingDirectory(
        this, tr("Select Mupen64 plugins directory"));
  }
  UpdateM64PluginDir();
}

void MainWindow::editedMupen64PluginDir()
{
  QString path;
  path = ui->le_PluginsDir->text();
  if (!QFile::exists(path))
  {
    QMessageBox::warning(this, tr("Directory does not exist"),
        tr("Please set the plugins directory to a valid directory."));
    return;
  }
  UpdateM64PluginDir();
}

void MainWindow::UpdateM64PluginDir()
{
  if (!Mupen64PluginDir.isEmpty())
  {
    // Save the chosen directory in CuteMupen config
    ConfigSection* cfg = GetSection("UI-CuteMupen");
    if (cfg)
    {
        cfg->getParamWithName("PluginDir")->setStringValue(Mupen64PluginDir.toStdString());
        saveConfig();
    }
    // Set label to the chosen directory
    ui->le_PluginsDir->setText(Mupen64PluginDir);
    // Tweak PATH or LD_LIBRARY depending on OS
    QString envSeparator;
    QString envLD;
    QString Mupen64PluginDirNative = Mupen64PluginDir;
#if defined(Q_WS_WIN)
  {
      envLD = "PATH";
      Mupen64PluginDirNative.replace(QChar('/'), QChar('\\'));
  }
#elif defined(Q_WS_MAC)
  {
      envLD = "DYLD_LIBRARY_PATH";
  }
#else
  {
      envLD = "LD_LIBRARY_PATH";
  }
#endif

    // Set current directory
    QDir::setCurrent(Mupen64PluginDir);

#if defined(Q_WS_WIN)
{
          Mupen64PluginDir = Mupen64PluginDirNative;
}
#endif

    // Set environment variable
    AddToEnvVar(envLD, Mupen64PluginDir);
  }
}

void MainWindow::chooseMupen64DataDir(bool skipDialog)
{
  if (!skipDialog)
  {
    Mupen64DataDir = QFileDialog::getExistingDirectory(this,
        tr("Select data directory"),
        Mupen64DataDir.isEmpty() ?
            (Mupen64PluginDir.isEmpty() ? "" : Mupen64PluginDir)
            : Mupen64DataDir);
  }
  UpdateM64DataDir();
}

void MainWindow::editedMupen64DataDir()
{
  QString path;
  path = ui->le_DataDir->text();
  if (!QFile::exists(path))
  {
    QMessageBox::warning(this, tr("Directory does not exist"),
        tr("Please set the data directory to a valid directory."));
    return;
  }
  UpdateM64DataDir();
}

void MainWindow::UpdateM64DataDir()
{
#if 0
  // Save the chosen directory in CuteMupen config
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");
  settings.setValue("Paths/Mupen64DataDir", Mupen64DataDir);
  // Set label to the chosen directory
  if (!Mupen64DataDir.isEmpty())
    ui->le_DataDir->setText(Mupen64DataDir);
#endif
}

void MainWindow::chooseMupen64ConfigDir(bool skipDialog)
{
  if (!skipDialog)
  {
    Mupen64ConfigDir = QFileDialog::getExistingDirectory(this,
        tr("Select configuration directory"),
        Mupen64ConfigDir.isEmpty() ?
            (Mupen64PluginDir.isEmpty() ? "" : Mupen64PluginDir)
            : Mupen64ConfigDir);
  }
  UpdateM64ConfigDir();
}

void MainWindow::editedMupen64ConfigDir()
{
  QString path;
  path = ui->le_ConfigDir->text();
  if (!QFile::exists(path))
  {
    QMessageBox::warning(this, tr("Directory does not exist"),
        tr("Please set the configuration directory to a valid directory."));
    return;
  }
  UpdateM64ConfigDir();
}

void MainWindow::UpdateM64ConfigDir()
{
#if 0
  // Save the chosen directory in CuteMupen config
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setIniCodec("UTF-8");
  settings.setValue("Paths/Mupen64ConfigDir", Mupen64ConfigDir);
  // Set label to the chosen directory
  if (!Mupen64ConfigDir.isEmpty())
    ui->le_ConfigDir->setText(Mupen64ConfigDir);
#endif
}

void MainWindow::chooseROMsDir(bool skipDialog)
{
  if (!skipDialog)
  {
    ROMsDir = QFileDialog::getExistingDirectory(
        this, tr("Select ROMs directory"),
        ROMsDir.isEmpty() ?
            (Mupen64PluginDir.isEmpty() ? QDir::currentPath() : Mupen64PluginDir)
            : ROMsDir);
  }
  UpdateROMsDir();
}

void MainWindow::editedROMsDir()
{
  QString path;
  path = ui->le_ROMsDir->text();      
  if (!QFile::exists(path))
  {
    QMessageBox::warning(this, tr("Directory does not exist"),
        tr("Please set the ROMs directory to a valid directory."));
    ui->le_ROMsDir->setText(QDir::currentPath());
    return;
  }
  UpdateROMsDir();
}

void MainWindow::UpdateROMsDir()
{
  if (!ROMsDir.isEmpty() && QFile::exists(ROMsDir))
  {
    // Save the chosen directory in CuteMupen config
    ConfigSection* cfg = GetSection("UI-CuteMupen");
    if (cfg)
    {
        cfg->getParamWithName("GamesPath")->setStringValue(ROMsDir.toStdString());
        saveConfig();
    }
    // Set the label to the chosen directory
    ui->le_ROMsDir->setText(ROMsDir);
    QStringList nameFilters;
    nameFilters << "*.n64" << "*.v64" << "*.z64" << "*.zip";
    fsmodel->setNameFilters(nameFilters);
    fsmodel->setFilter(
                QDir::Files|QDir::AllDirs|QDir::NoDotAndDotDot);
    fsmodel->setRootPath(QDir::currentPath());
    ui->treeView->setModel(fsmodel);
    ui->treeView->setRootIndex(fsmodel->index(ROMsDir));
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
  }
}

void MainWindow::AddToEnvVar (QString envVar, QString value)
{
    QString envSeparator;
#if defined(Q_WS_WIN)
  {
      envSeparator = ";";
  }
#else
  {
      envSeparator = ":";
  }
#endif
    // Set environment variable
    QString env = getenv(envVar.toLocal8Bit());
    QString newenv = envVar + "=" + env + envSeparator + value;
    const char* myEnv = newenv.toLocal8Bit().constData();
    putenv((char*)myEnv);
}
