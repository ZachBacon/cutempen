/********************************************************************************
** Form generated from reading UI file 'plugindialog.ui'
**
** Created: Sun Dec 26 20:41:29 2010
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
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_PluginDialogClass
{
public:
    QGridLayout *gridLayout;
    QGridLayout *gl_PluginParams;
    QPushButton *pb_Close;

    void setupUi(QDialog *PluginDialogClass)
    {
        if (PluginDialogClass->objectName().isEmpty())
            PluginDialogClass->setObjectName(QString::fromUtf8("PluginDialogClass"));
        PluginDialogClass->setWindowModality(Qt::ApplicationModal);
        PluginDialogClass->resize(400, 289);
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

        pb_Close = new QPushButton(PluginDialogClass);
        pb_Close->setObjectName(QString::fromUtf8("pb_Close"));

        gridLayout->addWidget(pb_Close, 1, 0, 1, 1);


        retranslateUi(PluginDialogClass);
        QObject::connect(pb_Close, SIGNAL(clicked()), PluginDialogClass, SLOT(accept()));

        QMetaObject::connectSlotsByName(PluginDialogClass);
    } // setupUi

    void retranslateUi(QDialog *PluginDialogClass)
    {
        PluginDialogClass->setWindowTitle(QApplication::translate("PluginDialogClass", "Plugin Settings", 0, QApplication::UnicodeUTF8));
        pb_Close->setText(QApplication::translate("PluginDialogClass", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PluginDialogClass: public Ui_PluginDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLUGINDIALOG_H
