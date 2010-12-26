#include "plugindialog.h"

#include <QDebug>

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>

PluginDialog::PluginDialog(QWidget *parent, const char* pluginName)
    : QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(pluginName);
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
      cbox->addItem("true");
      cbox->addItem("false");
      bval = (bool*)pValue;
      if (*bval)
        cbox->setCurrentIndex(cbox->findText("true"));
      else
        cbox->setCurrentIndex(cbox->findText("false"));
      ui.gl_PluginParams->addWidget(cbox, line, column + 1);
      break;
    case M64TYPE_FLOAT:
      dspbox = new QDoubleSpinBox ();
      dspbox->setMaximum(16000000);
      fval = (float*)pValue;
      dspbox->setValue (*fval);
      ui.gl_PluginParams->addWidget(dspbox, line, column + 1);
      break;
    case M64TYPE_INT:
      spbox = new QSpinBox ();
      spbox->setMaximum(65535);
      ival = (int*)pValue;
      spbox->setValue (*ival);
      ui.gl_PluginParams->addWidget(spbox, line, column + 1);
      break;
    case M64TYPE_STRING:
      ledit = new QLineEdit ((const char*)pValue);
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
