/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   CuteMupen - mainwindow.h                                              *
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QModelIndex>
#include <QDirModel>
#include <QString>

#include "m64p_types.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

    QString Mupen64Library;
    QString Mupen64PluginDir;
    QString Mupen64DataDir;
    QString Mupen64ConfigDir;
    QString ROMsDir;
    QString ROMFile;
    QDirModel *dirModel;

    m64p_error InitMupen64();
    void RestoreSettings();
    void ApplyConfiguration();
    bool LoadRom(QString& ROMFile);
    m64p_error DetachCoreLib();
    m64p_error OpenConfigurationHandles();
    m64p_error SaveConfigurationOptions();

    m64p_handle l_ConfigCore;
    m64p_handle l_ConfigUI;
    m64p_handle l_ConfigVideo;
    m64p_handle l_DataDirPath;
    m64p_handle l_ConfigDirPath;

public slots:
    void chooseMupen64Library (bool skipDialog = false);
    void chooseMupen64PluginDir(bool skipDialog = false);
    void chooseMupen64DataDir (bool skipDialog = false);
    void chooseMupen64ConfigDir (bool skipDialog = false);
    void chooseROMsDir(bool skipDialog = false);
    int clickedROM(const QModelIndex & index);
    int clickedRun();
    void toggledEmuMode(bool checked);
    void toggledLogging(bool checked);
};

#endif // MAINWINDOW_H
