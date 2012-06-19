/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   CuteMupen - inputdialog.cpp                                           *
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

#include "inputdialog.h"

#include <QDebug>

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>
#include <QGridLayout>

#include "mupen64plusplus/MupenAPI.h"
#include "mupen64plusplus/MupenAPIpp.h"

InputDialog::InputDialog(QWidget *parent, QVector<ConfigSection*> inpSections, QString pluginName)
    : QDialog(parent)
{
	ui.setupUi(this);
    pluginName.chop(pluginName.length() - pluginName.lastIndexOf('-'));
    this->setWindowTitle(pluginName);
    inputSections = inpSections;
    config = 0; // Will be the section for current tab
	line = 0;
	column = 0;
	tabIndex = 0;
	QString glName;

    for (tabIndex = 0; tabIndex < inputSections.size(); tabIndex++)
    {
        glName.sprintf("gl_Tab%d", tabIndex + 1);
        qgl = ui.tabWidget->findChild<QGridLayout*>(glName);
        config = inputSections[tabIndex];
        for (int i = 0; i < config->m_parameters.size(); i++)
            AddParameter(config->m_parameters[i]);
    }
}

InputDialog::~InputDialog()
{
}

void InputDialog::SetCurrentTab (int index)
{
  ui.tabWidget->setCurrentIndex(index);
  //config = inputSections[index - 1];
}

void InputDialog::NextTab ()
{
  tabIndex++;
  ui.tabWidget->setCurrentIndex(tabIndex);
  config = inputSections[tabIndex];
  QString glName;
  glName.sprintf("gl_Tab%d", tabIndex + 1);
  qgl = ui.tabWidget->findChild<QGridLayout*>(glName);
}

void InputDialog::AddParameter (ConfigParam &par)
{
  ui.tabWidget->setCurrentIndex(tabIndex);
  QString pName = QString::fromStdString(par.m_param_name);
  QLabel* label = new QLabel (pName);
  label->setToolTip(QString::fromStdString(par.m_help_string));
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  qgl->addWidget(label, line, column, Qt::AlignRight|Qt::AlignVCenter);

  QComboBox* cbox;
  QLineEdit* ledit;
  QDoubleSpinBox* dspbox;
  QSpinBox* spbox;


  switch (par.m_param_type)
  {
    case M64TYPE_BOOL:
      cbox = new QComboBox ();
      cbox->setObjectName(pName);
      cbox->addItem("true");
      cbox->addItem("false");
      if (par.getBoolValue())
        cbox->setCurrentIndex(cbox->findText("true"));
      else
        cbox->setCurrentIndex(cbox->findText("false"));
      cbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      qgl->addWidget(cbox, line, column + 1);
      connect(cbox, SIGNAL(currentIndexChanged(int)), this, SLOT(changedBoolSetting(int)));
      break;
    case M64TYPE_FLOAT:
      dspbox = new QDoubleSpinBox ();
      dspbox->setObjectName(pName);
      dspbox->setMaximum(16000000);
      dspbox->setValue (par.getFloatValue());
      dspbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      qgl->addWidget(dspbox, line, column + 1);
      connect(dspbox, SIGNAL(valueChanged(double)), this, SLOT(changedFloatSetting(double)));
      break;
    case M64TYPE_INT:
      spbox = new QSpinBox ();
      spbox->setObjectName(pName);
      spbox->setMaximum(32767);
      spbox->setMinimum(-32768);
      spbox->setValue (par.getIntValue());
      spbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      qgl->addWidget(spbox, line, column + 1);
      connect(spbox, SIGNAL(valueChanged(int)), this, SLOT(changedIntSetting(int)));
      break;
    case M64TYPE_STRING:
      QString tmp = QString::fromStdString(par.getStringValue());
      ledit = new QLineEdit (tmp.toLocal8Bit().constData());
      ledit->setObjectName(pName);
      ledit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      qgl->addWidget(ledit, line, column + 1);
      connect(ledit, SIGNAL(editingFinished()), this, SLOT(changedStringSetting()));
  }

  if (line < 11)
    line++;
  else
  {
    line = 0;
    column += 2;
  }
}

void InputDialog::changedBoolSetting (int index)
{
  bool newValue;
  QComboBox* cb = (QComboBox*)sender();
  if (cb->findText("true") == index)
    newValue = true;
  else
    newValue = false;

  ConfigParam* par = config->getParamWithName(
              sender()->objectName().toLocal8Bit().constData());
  if (par)
      par->setBoolValue(newValue);
  else
      qDebug () << "InputDialog : no param named" << sender()->objectName().toLocal8Bit().constData();
  //saveConfig();
}

void InputDialog::changedFloatSetting (double value)
{
  float newValue = (float) value;
  ConfigParam* par = config->getParamWithName(
              sender()->objectName().toLocal8Bit().constData());
  if (par)
      par->setFloatValue(newValue);
  else
      qDebug () << "InputDialog : no param named" << sender()->objectName().toLocal8Bit().constData();
  //saveConfig();
}

void InputDialog::changedIntSetting (int value)
{
    ConfigParam* par = config->getParamWithName(
                sender()->objectName().toLocal8Bit().constData());
    if (par)
        par->setIntValue(value);
    else
        qDebug () << "InputDialog : no param named" << sender()->objectName().toLocal8Bit().constData();
    //saveConfig();
}

void InputDialog::changedStringSetting ()
{
  QLineEdit* ledit = (QLineEdit*)sender();
  ConfigParam* par = config->getParamWithName(
              sender()->objectName().toLocal8Bit().constData());
  if (par)
      par->setStringValue(ledit->text().toLocal8Bit().constData());
  else
      qDebug () << "InputDialog : no param named" << sender()->objectName().toLocal8Bit().constData();
  //saveConfig();
}
