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

    for (int idx = 0; idx < inputSections.size(); idx++)
    {
        config = inputSections[idx];
        QWidget *page = new QWidget;
        QGridLayout *layout = new QGridLayout (page);
        for (size_t i = 0; i < config->m_parameters.size(); i++)
            AddParameter(layout, config->m_parameters[i]);
        QString tabText = QString::fromStdString(
                    config->getParamWithName("name")->getStringValue());
        ui.tabWidget->addTab(page, tabText);
    }
}

InputDialog::~InputDialog()
{
}

void InputDialog::AddParameter (QGridLayout *layout, ConfigParam &par)
{
  // First add a label showing parameter name, with parameter help text in its tooltip
  QString pName = QString::fromStdString(par.m_param_name);
  QLabel* label = new QLabel (pName);
  label->setToolTip(QString::fromStdString(par.m_help_string));
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  layout->addWidget(label, line, column, Qt::AlignRight|Qt::AlignVCenter);

  // Now show the parameter value in a widget depending on parameter type
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
      layout->addWidget(cbox, line, column + 1);
      connect(cbox, SIGNAL(currentIndexChanged(int)), this, SLOT(changedBoolSetting(int)));
      break;
    case M64TYPE_FLOAT:
      dspbox = new QDoubleSpinBox ();
      dspbox->setObjectName(pName);
      dspbox->setMaximum(16000000);
      dspbox->setValue (par.getFloatValue());
      dspbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      layout->addWidget(dspbox, line, column + 1);
      connect(dspbox, SIGNAL(valueChanged(double)), this, SLOT(changedFloatSetting(double)));
      if (pName.toLower() == "version")
          dspbox->setDisabled(true);
      break;
    case M64TYPE_INT:
      spbox = new QSpinBox ();
      spbox->setObjectName(pName);
      spbox->setMaximum(32767);
      spbox->setMinimum(-32768);
      spbox->setValue (par.getIntValue());
      spbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      layout->addWidget(spbox, line, column + 1);
      connect(spbox, SIGNAL(valueChanged(int)), this, SLOT(changedIntSetting(int)));
      break;
    case M64TYPE_STRING:
      QString value = QString::fromStdString(par.getStringValue());
      ledit = new QLineEdit (value.toLocal8Bit().constData());
      ledit->setObjectName(pName);
      ledit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      layout->addWidget(ledit, line, column + 1);
      connect(ledit, SIGNAL(editingFinished()), this, SLOT(changedStringSetting()));
      if (pName.toLower() == "name")
      {
          ui.tabWidget->setTabText(ui.tabWidget->currentIndex(), value);
          ledit->setDisabled(true);
      }
  }

  if (line < 11)
    line++;
  else
  {
    line = 0;
    column += 2;
  }
}

void InputDialog::currentChanged(int index)
{
    config = inputSections[index];
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
  saveConfig();
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
  saveConfig();
}

void InputDialog::changedIntSetting (int value)
{
    ConfigParam* par = config->getParamWithName(
                sender()->objectName().toLocal8Bit().constData());
    if (par)
        par->setIntValue(value);
    else
        qDebug () << "InputDialog : no param named" << sender()->objectName().toLocal8Bit().constData();
    saveConfig();
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
  saveConfig();
}
