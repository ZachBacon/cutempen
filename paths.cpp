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
#include <QMessageBox>
#include <QDir>

#include "osal/osal_preproc.h" // For default core library filename

extern "C" {
#include "m64p/core_interface.h"
}

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
  path = ui->le_Library->text();
  if (!QFile::exists(path))
  {
    QMessageBox::warning(this, tr("File does not exist"),
        tr("Please set the library file to a valid file."));
    ui->le_Library->setText(QDir::currentPath());
    return;
  }
  Mupen64Library = path;
  UpdateM64Library();
}

void MainWindow::UpdateM64Library()
{
  if (isCoreReady)
  {
    /* Shut down and release the Core library */
    (*CoreShutdown)();
    DetachCoreLib();
  }
  if (!Mupen64Library.isEmpty())
  {
    // Save the chosen directory in CuteMupen config
    QSettings settings ("CuteMupen", "CuteMupen");
    settings.setValue("Paths/Mupen64Library", Mupen64Library);
    // Set label to the chosen directory
    ui->le_Library->setText(Mupen64Library);
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
  ApplyConfiguration();
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
    QSettings settings ("CuteMupen", "CuteMupen");
    settings.setValue("Paths/Mupen64PluginDir", Mupen64PluginDir);
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

    //QString currentEnv (getenv(envLD.toLocal8Bit().constData()));
    //QMessageBox::information(this, "Now is Current " + envLD, currentEnv);
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
  // Save the chosen directory in CuteMupen config
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setValue("Paths/Mupen64DataDir", Mupen64DataDir);
  // Set label to the chosen directory
  if (!Mupen64DataDir.isEmpty())
    ui->le_DataDir->setText(Mupen64DataDir);
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
  // Save the chosen directory in CuteMupen config
  QSettings settings ("CuteMupen", "CuteMupen");
  settings.setValue("Paths/Mupen64ConfigDir", Mupen64ConfigDir);
  // Set label to the chosen directory
  if (!Mupen64ConfigDir.isEmpty())
    ui->le_ConfigDir->setText(Mupen64ConfigDir);
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
  if (!ROMsDir.isEmpty())
  {
    // Save the chosen directory in CuteMupen config
    QSettings settings ("CuteMupen", "CuteMupen");
    settings.setValue("Paths/ROMsDir", ROMsDir);
    // Set the label to the chosen directory
    ui->le_ROMsDir->setText(ROMsDir);
    // Update the ROM browser view
    dirModel->setSorting(QDir::DirsFirst);
    dirModel->setFilter(QDir::Files|QDir::AllDirs|QDir::NoDotAndDotDot);
    QStringList filters;
    filters << "*.n64" << "*.v64" << "*.z64";
    dirModel->setNameFilters(filters);
    QModelIndex idx = dirModel->index(ROMsDir, 0);
    ui->treeView->setModel(dirModel);
    ui->treeView->setRootIndex(idx);
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    //ui->treeView->expandAll();

    // (vk) for more recent Qt version (4.6+), use fsmodel
    //QFileSystemModel *fsmodel = new QFileSystemModel();
    //fsmodel->setRootDirectory(ROMsDir);
    //fsmodel->setRootPath(QDir::currentPath());
    //ui->treeView->setModel(fsmodel);
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
