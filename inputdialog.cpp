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

extern "C" {
#include "m64p_config.h"
}

extern ptr_ConfigSetParameter ConfigSetParameter;
extern ptr_ConfigGetParameterHelp ConfigGetParameterHelp;

InputDialog::InputDialog(QWidget *parent, m64p_handle handle[], QStringList pluginName)
    : QDialog(parent)
{
	ui.setupUi(this);
	pluginName[0].chop(pluginName[0].length() - pluginName[0].lastIndexOf('-'));
	this->setWindowTitle(pluginName[0]);
	cfgHandle[0] = &handle[0];
	line = 0;
	column = 0;
	tabIndex = 0;
	QString glName;
	glName.sprintf("gl_Tab%d", tabIndex + 1);
        qgl = ui.tabWidget->findChild<QGridLayout*>(glName);
}

InputDialog::~InputDialog()
{
}

void InputDialog::SetCurrentTab (int index)
{
  ui.tabWidget->setCurrentIndex(index);
}

void InputDialog::NextTab ()
{
  tabIndex++;
  ui.tabWidget->setCurrentIndex(tabIndex);
  QString glName;
  glName.sprintf("gl_Tab%d", tabIndex + 1);
  qgl = ui.tabWidget->findChild<QGridLayout*>(glName);
}

void InputDialog::AddParameter (const char* pName, m64p_type pType, void* pValue)
{
  ui.tabWidget->setCurrentIndex(tabIndex);
  QLabel* label = new QLabel (pName);
  label->setToolTip((*ConfigGetParameterHelp)(cfgHandle[tabIndex], pName));
  qgl->addWidget(label, line, column, Qt::AlignRight|Qt::AlignVCenter);

  QComboBox* cbox;
  QLineEdit* ledit;
  QDoubleSpinBox* dspbox;
  QSpinBox* spbox;

  bool* bval;
  float* fval;
  int* ival;

  switch (pType)
  {
    case M64TYPE_BOOL:
      cbox = new QComboBox ();
      cbox->setObjectName(pName);
      cbox->setToolTip((*ConfigGetParameterHelp)(cfgHandle[tabIndex], pName));
      cbox->addItem("true");
      cbox->addItem("false");
      bval = (bool*)pValue;
      if (*bval)
        cbox->setCurrentIndex(cbox->findText("true"));
      else
        cbox->setCurrentIndex(cbox->findText("false"));
      qgl->addWidget(cbox, line, column + 1);
      connect(cbox, SIGNAL(currentIndexChanged(int)), this, SLOT(changedBoolSetting(int)));
      break;
    case M64TYPE_FLOAT:
      dspbox = new QDoubleSpinBox ();
      dspbox->setObjectName(pName);
      dspbox->setToolTip((*ConfigGetParameterHelp)(cfgHandle[tabIndex], pName));
      dspbox->setMaximum(16000000);
      fval = (float*)pValue;
      dspbox->setValue (*fval);
      qgl->addWidget(dspbox, line, column + 1);
      connect(dspbox, SIGNAL(valueChanged(double)), this, SLOT(changedFloatSetting(double)));
      break;
    case M64TYPE_INT:
      spbox = new QSpinBox ();
      spbox->setObjectName(pName);
      spbox->setToolTip((*ConfigGetParameterHelp)(cfgHandle[tabIndex], pName));
      spbox->setMaximum(32767);
      spbox->setMinimum(-32768);
      ival = (int*)pValue;
      spbox->setValue (*ival);
      qgl->addWidget(spbox, line, column + 1);
      connect(spbox, SIGNAL(valueChanged(int)), this, SLOT(changedIntSetting(int)));
      break;
    case M64TYPE_STRING:
      QString tmp ((const char*)pValue);
      ledit = new QLineEdit (tmp.toLocal8Bit().constData());
      ledit->setObjectName(pName);
      ledit->setToolTip((*ConfigGetParameterHelp)(cfgHandle[tabIndex], pName));
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
  //qDebug () << sender()->objectName() << "=" << (newValue ? "true" : "false");
  (*ConfigSetParameter)(cfgHandle[ui.tabWidget->currentIndex()],
      sender()->objectName().toLocal8Bit().constData(),
      M64TYPE_BOOL,
      &newValue);
}

void InputDialog::changedFloatSetting (double value)
{
  float newValue = (float) value;
  //qDebug () << "Float param" << sender()->objectName().toLocal8Bit().constData() << "=" << newValue;
  (*ConfigSetParameter)(cfgHandle[ui.tabWidget->currentIndex()],
      sender()->objectName().toLocal8Bit().constData(),
      M64TYPE_FLOAT,
      &newValue);
}

void InputDialog::changedIntSetting (int value)
{
  //qDebug () << "Int param" << sender()->objectName().toLocal8Bit().constData() << "=" << value;
  (*ConfigSetParameter)(cfgHandle[ui.tabWidget->currentIndex()],
      sender()->objectName().toLocal8Bit().constData(),
      M64TYPE_INT,
      &value);
}

void InputDialog::changedStringSetting ()
{
  QLineEdit* ledit = (QLineEdit*)sender();
  //qDebug () << "String param" << sender()->objectName().toLocal8Bit().constData() << "=" << ledit->text();
  (*ConfigSetParameter)(cfgHandle[ui.tabWidget->currentIndex()],
      sender()->objectName().toLocal8Bit().constData(),
      M64TYPE_STRING,
      ledit->text().toLocal8Bit().constData());
}
