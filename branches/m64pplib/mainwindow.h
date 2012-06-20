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
#include <QString>
#include <QVector>
#include <QFileSystemModel>

#include "mupen64plusplus/MupenAPI.h"
#include "mupen64plusplus/MupenAPIpp.h"

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

    Mupen64PlusPlus* m_api;

    QString Mupen64Library;
    QString Mupen64PluginDir;
    QString Mupen64DataDir;
    QString Mupen64ConfigDir;
    QString ROMsDir;
    QString ROMFile;
    QFileSystemModel *fsmodel;

    std::vector<ConfigSection> configSections;
    ConfigSection* GetSection (const char* name);
    ConfigSection* GetSection (m64p_handle);

    m64p_error InitMupen64();
    void RestoreSettings();
    bool LoadFile(QString& ROMFile);
    bool LoadRom (qint64 lentgh, char* buffer);
    m64p_error DetachCoreLib();

    void UpdateM64Library ();
    void UpdateM64PluginDir ();
    void UpdateM64DataDir ();
    void UpdateM64ConfigDir ();
    void UpdateROMsDir ();
    void AddToEnvVar (QString envVar, QString value);

    void FlushLog ();

    bool isCoreReady;

public slots:
    void chooseMupen64Library (bool skipDialog = false);
    void editedMupen64Library ();
    void chooseMupen64PluginDir(bool skipDialog = false);
    void editedMupen64PluginDir();
    void chooseMupen64DataDir (bool skipDialog = false);
    void editedMupen64DataDir();
    void chooseMupen64ConfigDir (bool skipDialog = false);
    void editedMupen64ConfigDir();
    void chooseROMsDir(bool skipDialog = false);
    void editedROMsDir();
    int clickedROM(const QModelIndex & index);
    int clickedRun();
    void chooseResolution (QString text);
    void toggledEmuMode(bool checked);
    void toggledLogging(bool checked);
    void toggledOSD(bool checked);
    void toggledFullscreen(bool checked);
    void chooseGfxPlugin (QString text);
    void chooseSndPlugin (QString text);
    void chooseInpPlugin (QString text);
    void chooseRspPlugin (QString text);
    void clickedGfx ();
    void clickedSnd ();
    void clickedInp ();
    void clickedRsp ();
};

#endif // MAINWINDOW_H
