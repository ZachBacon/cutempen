/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   CuteMupen - plugindialog.cpp                                          *
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

#include "plugindialog.h"

#include <QDebug>

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include "mupen64plusplus/MupenAPI.h"
#include "mupen64plusplus/MupenAPIpp.h"

PluginDialog::PluginDialog(QWidget *parent, ConfigSection* cfg)
    : QDialog(parent)
{
	ui.setupUi(this);
    if (cfg)
        this->setWindowTitle(
                QString::fromStdString(cfg->m_section_name));
    config = cfg;
	line = 0;
	column = 0;

    for (size_t idx = 0; idx < cfg->m_parameters.size(); idx++)
        AddParameter(cfg->m_parameters[idx]);
}

PluginDialog::~PluginDialog()
{

}

void PluginDialog::AddParameter (ConfigParam& par)
{
  // Fill a label with param name and set tooltip with param help text
  QLabel* label = new QLabel (QString::fromStdString(par.m_param_name));
  label->setToolTip(QString::fromStdString(par.m_help_string));
  ui.gl_PluginParams->addWidget(label, line, column, Qt::AlignRight|Qt::AlignVCenter);

  QComboBox* cbox;
  QLineEdit* ledit;
  QDoubleSpinBox* dspbox;
  QSpinBox* spbox;

  // Add a widget with param value depending on its type
  switch (par.m_param_type)
  {
    case M64TYPE_BOOL:
      cbox = new QComboBox ();
      cbox->setObjectName(QString::fromStdString(par.m_param_name));
      cbox->addItem("true");
      cbox->addItem("false");
      if (par.getBoolValue())
        cbox->setCurrentIndex(cbox->findText("true"));
      else
        cbox->setCurrentIndex(cbox->findText("false"));
      ui.gl_PluginParams->addWidget(cbox, line, column + 1);
      connect(cbox, SIGNAL(currentIndexChanged(int)), this, SLOT(changedBoolSetting(int)));
      break;
    case M64TYPE_FLOAT:
      dspbox = new QDoubleSpinBox ();
      dspbox->setObjectName(QString::fromStdString(par.m_param_name));
      dspbox->setMaximum(16000000);
      dspbox->setValue (par.getFloatValue());
      ui.gl_PluginParams->addWidget(dspbox, line, column + 1);
      connect(dspbox, SIGNAL(valueChanged(double)), this, SLOT(changedFloatSetting(double)));
      break;
    case M64TYPE_INT:
      spbox = new QSpinBox ();
      spbox->setObjectName(QString::fromStdString(par.m_param_name));
      spbox->setMaximum(65535);
      spbox->setValue (par.getIntValue());
      ui.gl_PluginParams->addWidget(spbox, line, column + 1);
      connect(spbox, SIGNAL(valueChanged(int)), this, SLOT(changedIntSetting(int)));
      break;
    case M64TYPE_STRING:
      ledit = new QLineEdit (QString::fromStdString(par.getStringValue()));
      ledit->setObjectName(QString::fromStdString(par.m_param_name));
      ui.gl_PluginParams->addWidget(ledit, line, column + 1);
      connect(ledit, SIGNAL(editingFinished()), this, SLOT(changedStringSetting()));
  }
  if (line < 12)
    line++;
  else
  {
    line = 0;
    column += 2;
  }
}

void PluginDialog::changedBoolSetting (int index)
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
      qDebug () << "PluginDialog : no param named" << sender()->objectName().toLocal8Bit().constData();
  saveConfig();
}

void PluginDialog::changedFloatSetting (double value)
{
  float newValue = (float) value;
  ConfigParam* par = config->getParamWithName(
              sender()->objectName().toLocal8Bit().constData());
  if (par)
      par->setFloatValue(newValue);
  else
      qDebug () << "PluginDialog : no param named" << sender()->objectName().toLocal8Bit().constData();
  saveConfig();
}

void PluginDialog::changedIntSetting (int value)
{
    ConfigParam* par = config->getParamWithName(
                sender()->objectName().toLocal8Bit().constData());
    if (par)
        par->setIntValue(value);
    else
        qDebug () << "PluginDialog : no param named" << sender()->objectName().toLocal8Bit().constData();
    saveConfig();
}

void PluginDialog::changedStringSetting ()
{
  QLineEdit* ledit = (QLineEdit*)sender();
  ConfigParam* par = config->getParamWithName(
              sender()->objectName().toLocal8Bit().constData());
  if (par)
      par->setStringValue(ledit->text().toLocal8Bit().constData());
  else
      qDebug () << "PluginDialog : no param named" << sender()->objectName().toLocal8Bit().constData();
  saveConfig();
}
