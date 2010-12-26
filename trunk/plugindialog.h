#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QtGui/QDialog>
#include "ui_plugindialog.h"

#include "m64p_types.h"

class PluginDialog : public QDialog
{
    Q_OBJECT

public:
    PluginDialog(QWidget *parent = 0, m64p_handle handle = 0, const char* pluginName = 0);
    ~PluginDialog();

    void AddParameter (const char* pName, m64p_type pType, void* pValue);

private:
    Ui::PluginDialogClass ui;
    m64p_handle cfgHandle;
    int line;
    int column;

public slots:
    void changedBoolSetting (int index);
    void changedFloatSetting (double value);
    void changedIntSetting (int value);
    void changedStringSetting ();

signals:
    void boolValueChanged ();
    void floatValueChanged ();
    void intValueChanged ();
    void stringValueChanged ();
};

#endif // PLUGINDIALOG_H
