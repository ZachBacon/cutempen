#include "plugindialog.h"

#include <QDebug>

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>

PluginDialog::PluginDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	line = 0;
	column = 0;
}

PluginDialog::~PluginDialog()
{

}

void PluginDialog::AddParameter (const char* pName, m64p_type pType, void* pValue)
{
  QHBoxLayout* hbox = new QHBoxLayout ();
  QLabel* label = new QLabel (pName);
  hbox->addWidget(label, 0, Qt::AlignRight|Qt::AlignVCenter);
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
      hbox->addWidget(cbox);
      break;
    case M64TYPE_FLOAT:
      //ledit = new QLineEdit (QString((float)pValue));
      //hbox->addWidget(ledit);
      dspbox = new QDoubleSpinBox ();
      fval = (float*)pValue;
      dspbox->setValue (*fval);
      hbox->addWidget (dspbox);
      break;
    case M64TYPE_INT:
      spbox = new QSpinBox ();
      ival = (int*)pValue;
      spbox->setValue (*ival);
      hbox->addWidget (spbox);
      break;

    case M64TYPE_STRING:
      ledit = new QLineEdit ((const char*)pValue);
      //ledit->setText((const char*)(pValue));
      hbox->addWidget(ledit);
  }


  hbox->insertSpacing(1, 100);
  ui.gl_PluginParams->addLayout(hbox, line, column);
  //ui.gl_PluginParams->

  //qDebug () << "PluginDialog::AddParameter(" << pName << ")";
  if (line < 10)
    line++;
  else
  {
    line = 0;
    column ++;
  }
}
