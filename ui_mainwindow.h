/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Sat Mar 6 11:53:14 2010
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *layoutWidget;
    QGridLayout *gl_Main;
    QPushButton *pb_Run;
    QPushButton *pb_Quit;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tab_Paths;
    QWidget *tab;
    QWidget *layoutWidget1;
    QGridLayout *gl_Paths;
    QPushButton *pb_Library;
    QLabel *lb_Library;
    QPushButton *pb_Plugins;
    QLabel *lb_Program;
    QPushButton *pb_DataDir;
    QLabel *lb_DataDir;
    QPushButton *pb_ConfigDir;
    QLabel *lb_ConfigDir;
    QPushButton *pb_ROMdir;
    QLabel *lb_ROMsDirectory;
    QWidget *tab_Settings;
    QWidget *layoutWidget2;
    QGridLayout *gl_Settings;
    QCheckBox *cb_Fullscreen;
    QComboBox *cb_Resolution;
    QCheckBox *cb_OSD;
    QSpacerItem *horizontalSpacer_2;
    QWidget *tab_ROMs;
    QTreeView *treeView;
    QWidget *tab_About;
    QWidget *layoutWidget3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_3;
    QLabel *lb_About_NameVersion;
    QSpacerItem *horizontalSpacer_4;
    QTextBrowser *txt_About;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(600, 400);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 581, 361));
        gl_Main = new QGridLayout(layoutWidget);
        gl_Main->setSpacing(6);
        gl_Main->setMargin(11);
        gl_Main->setObjectName(QString::fromUtf8("gl_Main"));
        gl_Main->setSizeConstraint(QLayout::SetDefaultConstraint);
        gl_Main->setContentsMargins(0, 0, 0, 0);
        pb_Run = new QPushButton(layoutWidget);
        pb_Run->setObjectName(QString::fromUtf8("pb_Run"));

        gl_Main->addWidget(pb_Run, 2, 0, 1, 1);

        pb_Quit = new QPushButton(layoutWidget);
        pb_Quit->setObjectName(QString::fromUtf8("pb_Quit"));

        gl_Main->addWidget(pb_Quit, 2, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gl_Main->addItem(horizontalSpacer, 2, 2, 1, 1);

        tab_Paths = new QTabWidget(layoutWidget);
        tab_Paths->setObjectName(QString::fromUtf8("tab_Paths"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(tab_Paths->sizePolicy().hasHeightForWidth());
        tab_Paths->setSizePolicy(sizePolicy);
        tab_Paths->setMaximumSize(QSize(16777215, 16777215));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        layoutWidget1 = new QWidget(tab);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 10, 541, 231));
        gl_Paths = new QGridLayout(layoutWidget1);
        gl_Paths->setSpacing(6);
        gl_Paths->setMargin(11);
        gl_Paths->setObjectName(QString::fromUtf8("gl_Paths"));
        gl_Paths->setContentsMargins(0, 0, 0, 0);
        pb_Library = new QPushButton(layoutWidget1);
        pb_Library->setObjectName(QString::fromUtf8("pb_Library"));

        gl_Paths->addWidget(pb_Library, 0, 0, 1, 1);

        lb_Library = new QLabel(layoutWidget1);
        lb_Library->setObjectName(QString::fromUtf8("lb_Library"));

        gl_Paths->addWidget(lb_Library, 0, 1, 1, 1);

        pb_Plugins = new QPushButton(layoutWidget1);
        pb_Plugins->setObjectName(QString::fromUtf8("pb_Plugins"));

        gl_Paths->addWidget(pb_Plugins, 1, 0, 1, 1);

        lb_Program = new QLabel(layoutWidget1);
        lb_Program->setObjectName(QString::fromUtf8("lb_Program"));

        gl_Paths->addWidget(lb_Program, 1, 1, 1, 1);

        pb_DataDir = new QPushButton(layoutWidget1);
        pb_DataDir->setObjectName(QString::fromUtf8("pb_DataDir"));

        gl_Paths->addWidget(pb_DataDir, 2, 0, 1, 1);

        lb_DataDir = new QLabel(layoutWidget1);
        lb_DataDir->setObjectName(QString::fromUtf8("lb_DataDir"));

        gl_Paths->addWidget(lb_DataDir, 2, 1, 1, 1);

        pb_ConfigDir = new QPushButton(layoutWidget1);
        pb_ConfigDir->setObjectName(QString::fromUtf8("pb_ConfigDir"));

        gl_Paths->addWidget(pb_ConfigDir, 3, 0, 1, 1);

        lb_ConfigDir = new QLabel(layoutWidget1);
        lb_ConfigDir->setObjectName(QString::fromUtf8("lb_ConfigDir"));

        gl_Paths->addWidget(lb_ConfigDir, 3, 1, 1, 1);

        pb_ROMdir = new QPushButton(layoutWidget1);
        pb_ROMdir->setObjectName(QString::fromUtf8("pb_ROMdir"));

        gl_Paths->addWidget(pb_ROMdir, 4, 0, 1, 1);

        lb_ROMsDirectory = new QLabel(layoutWidget1);
        lb_ROMsDirectory->setObjectName(QString::fromUtf8("lb_ROMsDirectory"));

        gl_Paths->addWidget(lb_ROMsDirectory, 4, 1, 1, 1);

        tab_Paths->addTab(tab, QString());
        tab_Settings = new QWidget();
        tab_Settings->setObjectName(QString::fromUtf8("tab_Settings"));
        layoutWidget2 = new QWidget(tab_Settings);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(10, 10, 551, 71));
        gl_Settings = new QGridLayout(layoutWidget2);
        gl_Settings->setSpacing(6);
        gl_Settings->setMargin(11);
        gl_Settings->setObjectName(QString::fromUtf8("gl_Settings"));
        gl_Settings->setVerticalSpacing(6);
        gl_Settings->setContentsMargins(0, 0, 0, 0);
        cb_Fullscreen = new QCheckBox(layoutWidget2);
        cb_Fullscreen->setObjectName(QString::fromUtf8("cb_Fullscreen"));

        gl_Settings->addWidget(cb_Fullscreen, 0, 0, 1, 1);

        cb_Resolution = new QComboBox(layoutWidget2);
        cb_Resolution->setObjectName(QString::fromUtf8("cb_Resolution"));
        cb_Resolution->setMaximumSize(QSize(110, 16777215));
        cb_Resolution->setEditable(true);

        gl_Settings->addWidget(cb_Resolution, 0, 1, 1, 1);

        cb_OSD = new QCheckBox(layoutWidget2);
        cb_OSD->setObjectName(QString::fromUtf8("cb_OSD"));

        gl_Settings->addWidget(cb_OSD, 1, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gl_Settings->addItem(horizontalSpacer_2, 0, 2, 1, 1);

        tab_Paths->addTab(tab_Settings, QString());
        tab_ROMs = new QWidget();
        tab_ROMs->setObjectName(QString::fromUtf8("tab_ROMs"));
        treeView = new QTreeView(tab_ROMs);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        treeView->setGeometry(QRect(10, 10, 561, 281));
        QFont font;
        font.setFamily(QString::fromUtf8("AlArabiya"));
        font.setPointSize(12);
        treeView->setFont(font);
        treeView->setAlternatingRowColors(true);
        treeView->setUniformRowHeights(true);
        treeView->header()->setVisible(false);
        tab_Paths->addTab(tab_ROMs, QString());
        tab_About = new QWidget();
        tab_About->setObjectName(QString::fromUtf8("tab_About"));
        layoutWidget3 = new QWidget(tab_About);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(11, 10, 521, 231));
        verticalLayout = new QVBoxLayout(layoutWidget3);
        verticalLayout->setSpacing(6);
        verticalLayout->setMargin(11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        lb_About_NameVersion = new QLabel(layoutWidget3);
        lb_About_NameVersion->setObjectName(QString::fromUtf8("lb_About_NameVersion"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lb_About_NameVersion->sizePolicy().hasHeightForWidth());
        lb_About_NameVersion->setSizePolicy(sizePolicy1);
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        lb_About_NameVersion->setFont(font1);
        lb_About_NameVersion->setText(QString::fromUtf8("CuteMupen Version"));

        horizontalLayout->addWidget(lb_About_NameVersion);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout);

        txt_About = new QTextBrowser(layoutWidget3);
        txt_About->setObjectName(QString::fromUtf8("txt_About"));
        txt_About->setSource(QUrl("qrc:/about/resources/about.html"));
        txt_About->setOpenExternalLinks(true);

        verticalLayout->addWidget(txt_About);

        tab_Paths->addTab(tab_About, QString());

        gl_Main->addWidget(tab_Paths, 0, 0, 1, 4);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);
        QObject::connect(pb_Quit, SIGNAL(clicked()), MainWindow, SLOT(close()));
        QObject::connect(pb_Run, SIGNAL(clicked()), MainWindow, SLOT(clickedRun()));
        QObject::connect(treeView, SIGNAL(clicked(QModelIndex)), MainWindow, SLOT(clickedROM(QModelIndex)));
        QObject::connect(pb_Library, SIGNAL(clicked()), MainWindow, SLOT(chooseMupen64Library()));
        QObject::connect(pb_Plugins, SIGNAL(clicked()), MainWindow, SLOT(chooseMupen64PluginDir()));
        QObject::connect(pb_DataDir, SIGNAL(clicked()), MainWindow, SLOT(chooseMupen64DataDir()));
        QObject::connect(pb_ConfigDir, SIGNAL(clicked()), MainWindow, SLOT(chooseMupen64ConfigDir()));
        QObject::connect(pb_ROMdir, SIGNAL(clicked()), MainWindow, SLOT(chooseROMsDir()));

        tab_Paths->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "CuteMupen", 0, QApplication::UnicodeUTF8));
        pb_Run->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        pb_Quit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        pb_Library->setText(QApplication::translate("MainWindow", "Mupen64 library", 0, QApplication::UnicodeUTF8));
        lb_Library->setText(QApplication::translate("MainWindow", "Select Mupen64Plus library file", 0, QApplication::UnicodeUTF8));
        pb_Plugins->setText(QApplication::translate("MainWindow", "Mupen64 plugins", 0, QApplication::UnicodeUTF8));
        lb_Program->setText(QApplication::translate("MainWindow", "Select Mupen64 directory", 0, QApplication::UnicodeUTF8));
        pb_DataDir->setText(QApplication::translate("MainWindow", "Mupen64 data", 0, QApplication::UnicodeUTF8));
        lb_DataDir->setText(QApplication::translate("MainWindow", "Select Mupen64 data directory (.ini files, lang, ...)", 0, QApplication::UnicodeUTF8));
        pb_ConfigDir->setText(QApplication::translate("MainWindow", "Mupen64 config", 0, QApplication::UnicodeUTF8));
        lb_ConfigDir->setText(QApplication::translate("MainWindow", "Select Mupen64 configuration directory", 0, QApplication::UnicodeUTF8));
        pb_ROMdir->setText(QApplication::translate("MainWindow", "ROM directory", 0, QApplication::UnicodeUTF8));
        lb_ROMsDirectory->setText(QApplication::translate("MainWindow", "Select directory containing ROMs", 0, QApplication::UnicodeUTF8));
        tab_Paths->setTabText(tab_Paths->indexOf(tab), QApplication::translate("MainWindow", "Paths", 0, QApplication::UnicodeUTF8));
        cb_Fullscreen->setText(QApplication::translate("MainWindow", "Fullscreen", 0, QApplication::UnicodeUTF8));
        cb_Resolution->clear();
        cb_Resolution->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "640x480", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "720x480", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "800x600", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1024x768", 0, QApplication::UnicodeUTF8)
        );
        cb_OSD->setText(QApplication::translate("MainWindow", "On Screen Display", 0, QApplication::UnicodeUTF8));
        tab_Paths->setTabText(tab_Paths->indexOf(tab_Settings), QApplication::translate("MainWindow", "Settings", 0, QApplication::UnicodeUTF8));
        tab_Paths->setTabText(tab_Paths->indexOf(tab_ROMs), QApplication::translate("MainWindow", "ROMs", 0, QApplication::UnicodeUTF8));
        tab_Paths->setTabText(tab_Paths->indexOf(tab_About), QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
