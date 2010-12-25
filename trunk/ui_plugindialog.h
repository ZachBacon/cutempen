/********************************************************************************
** Form generated from reading UI file 'plugindialog.ui'
**
** Created: Sat Dec 25 18:53:43 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLUGINDIALOG_H
#define UI_PLUGINDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_PluginDialogClass
{
public:
    QGridLayout *gridLayout;
    QGridLayout *gl_PluginParams;

    void setupUi(QDialog *PluginDialogClass)
    {
        if (PluginDialogClass->objectName().isEmpty())
            PluginDialogClass->setObjectName(QString::fromUtf8("PluginDialogClass"));
        PluginDialogClass->setWindowModality(Qt::ApplicationModal);
        PluginDialogClass->resize(400, 249);
        PluginDialogClass->setModal(true);
        gridLayout = new QGridLayout(PluginDialogClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gl_PluginParams = new QGridLayout();
        gl_PluginParams->setSpacing(6);
        gl_PluginParams->setObjectName(QString::fromUtf8("gl_PluginParams"));
        gl_PluginParams->setSizeConstraint(QLayout::SetDefaultConstraint);

        gridLayout->addLayout(gl_PluginParams, 0, 0, 1, 1);


        retranslateUi(PluginDialogClass);

        QMetaObject::connectSlotsByName(PluginDialogClass);
    } // setupUi

    void retranslateUi(QDialog *PluginDialogClass)
    {
        PluginDialogClass->setWindowTitle(QApplication::translate("PluginDialogClass", "Plugin Settings", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PluginDialogClass: public Ui_PluginDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLUGINDIALOG_H
