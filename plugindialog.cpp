#include "plugindialog.h"

#include <QDebug>

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>

extern "C" {
#include "m64p_config.h"
}

extern ptr_ConfigSetParameter ConfigSetParameter;

PluginDialog::PluginDialog(QWidget *parent, m64p_handle handle, const char* pluginName)
    : QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(pluginName);
	cfgHandle = handle;
	line = 0;
	column = 0;
}

PluginDialog::~PluginDialog()
{

}

void PluginDialog::AddParameter (const char* pName, m64p_type pType, void* pValue)
{
  QLabel* label = new QLabel (pName);
  ui.gl_PluginParams->addWidget(label, line, column, Qt::AlignRight|Qt::AlignVCenter);
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
      cbox->addItem("true");
      cbox->addItem("false");
      bval = (bool*)pValue;
      if (*bval)
        cbox->setCurrentIndex(cbox->findText("true"));
      else
        cbox->setCurrentIndex(cbox->findText("false"));
      ui.gl_PluginParams->addWidget(cbox, line, column + 1);
      connect(cbox, SIGNAL(currentIndexChanged(int)), this, SLOT(changedBoolSetting(int)));

      break;
    case M64TYPE_FLOAT:
      dspbox = new QDoubleSpinBox ();
      dspbox->setObjectName(pName);
      dspbox->setMaximum(16000000);
      fval = (float*)pValue;
      dspbox->setValue (*fval);
      ui.gl_PluginParams->addWidget(dspbox, line, column + 1);
      connect(dspbox, SIGNAL(valueChanged(double)), this, SLOT(changedFloatSetting(float)));
      break;
    case M64TYPE_INT:
      spbox = new QSpinBox ();
      spbox->setObjectName(pName);
      spbox->setMaximum(65535);
      ival = (int*)pValue;
      spbox->setValue (*ival);
      ui.gl_PluginParams->addWidget(spbox, line, column + 1);
      connect(spbox, SIGNAL(valueChanged(int)), this, SLOT(changedIntSetting(int)));
      break;
    case M64TYPE_STRING:
      ledit = new QLineEdit ((const char*)pValue);
      ledit->setObjectName(pName);
      ui.gl_PluginParams->addWidget(ledit, line, column + 1);
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
  qDebug () << sender()->objectName() << "=" << (newValue ? "true" : "false");
  (*ConfigSetParameter)(cfgHandle,
      sender()->objectName().toLocal8Bit().constData(),
      M64TYPE_BOOL,
      &newValue);
}

void PluginDialog::changedFloatSetting (double value)
{
  float newValue = (float) value;
  qDebug () << "Float param" << sender()->objectName().toLocal8Bit().constData() << "=" << newValue;
  (*ConfigSetParameter)(cfgHandle,
      sender()->objectName().toLocal8Bit().constData(),
      M64TYPE_FLOAT,
      &newValue);
}

void PluginDialog::changedIntSetting (int value)
{
  qDebug () << "Int param" << sender()->objectName().toLocal8Bit().constData() << "=" << value;
  (*ConfigSetParameter)(cfgHandle,
      sender()->objectName().toLocal8Bit().constData(),
      M64TYPE_INT,
      &value);
}

void PluginDialog::changedStringSetting ()
{

}
