#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QtGui/QDialog>
#include "ui_inputdialog.h"

#include <QDebug>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>

#include "m64p_types.h"

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    InputDialog(QWidget *parent = 0, m64p_handle handle[] = 0, QStringList sections = QStringList ());
    ~InputDialog();
    void NextTab ();
    void AddParameter (const char* pName, m64p_type pType, void* pValue);
    void SetCurrentTab (int index);

private:
    Ui::InputDialogClass ui;

    m64p_handle cfgHandle[4];
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
