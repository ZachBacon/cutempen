/********************************************************************************
** Form generated from reading UI file 'inputdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INPUTDIALOG_H
#define UI_INPUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>

QT_BEGIN_NAMESPACE

class Ui_InputDialogClass
{
public:
    QGridLayout *gridLayout;
    QGridLayout *gl_InputParams;
    QTabWidget *tabWidget;
    QPushButton *pb_Close;

    void setupUi(QDialog *InputDialogClass)
    {
        if (InputDialogClass->objectName().isEmpty())
            InputDialogClass->setObjectName(QString::fromUtf8("InputDialogClass"));
        InputDialogClass->setWindowModality(Qt::ApplicationModal);
        InputDialogClass->resize(521, 442);
        gridLayout = new QGridLayout(InputDialogClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gl_InputParams = new QGridLayout();
        gl_InputParams->setSpacing(6);
        gl_InputParams->setObjectName(QString::fromUtf8("gl_InputParams"));
        gl_InputParams->setSizeConstraint(QLayout::SetDefaultConstraint);
        tabWidget = new QTabWidget(InputDialogClass);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);

        gl_InputParams->addWidget(tabWidget, 0, 0, 1, 1);


        gridLayout->addLayout(gl_InputParams, 0, 0, 1, 1);

        pb_Close = new QPushButton(InputDialogClass);
        pb_Close->setObjectName(QString::fromUtf8("pb_Close"));

        gridLayout->addWidget(pb_Close, 1, 0, 1, 1);


        retranslateUi(InputDialogClass);
        QObject::connect(pb_Close, SIGNAL(clicked()), InputDialogClass, SLOT(accept()));

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(InputDialogClass);
    } // setupUi

    void retranslateUi(QDialog *InputDialogClass)
    {
        InputDialogClass->setWindowTitle(QCoreApplication::translate("InputDialogClass", "InputDialog", nullptr));
        pb_Close->setText(QCoreApplication::translate("InputDialogClass", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InputDialogClass: public Ui_InputDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INPUTDIALOG_H
