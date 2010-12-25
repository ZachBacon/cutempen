#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QtGui/QDialog>
#include "ui_plugindialog.h"

#include "m64p_types.h"

class PluginDialog : public QDialog
{
    Q_OBJECT

public:
    PluginDialog(QWidget *parent = 0);
    ~PluginDialog();

    void AddParameter (const char* pName, m64p_type pType, void* pValue);

private:
    Ui::PluginDialogClass ui;
    int line;
    int column;
};

#endif // PLUGINDIALOG_H
