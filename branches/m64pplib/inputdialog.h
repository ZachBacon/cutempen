/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   CuteMupen - inputdialog.h                                             *
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

#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QtGui/QDialog>
#include "ui_inputdialog.h"

#include <QDebug>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QVector>

#include "m64p_types.h"

class ConfigParam;
class ConfigSection;

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    InputDialog(QWidget *parent = 0, QVector<ConfigSection*> inpSections = QVector<ConfigSection*>(0), QString pluginName = 0);
    ~InputDialog();
    void NextTab ();
    void AddParameter (ConfigParam& par);
    void SetCurrentTab (int index);

private:
    Ui::InputDialogClass ui;

    QVector<ConfigSection*> inputSections;
    ConfigSection* config;
    QTabWidget* tab;
    QGridLayout* qgl;

    int line;
    int column;
    int tabIndex;

public slots:
    void changedBoolSetting (int index);
    void changedFloatSetting (double value);
    void changedIntSetting (int value);
    void changedStringSetting ();
};

#endif // INPUTDIALOG_H
