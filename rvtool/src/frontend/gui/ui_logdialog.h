/********************************************************************************
** Form generated from reading UI file 'logdialog.ui'
**
** Created: Thu May 30 23:20:37 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGDIALOG_H
#define UI_LOGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LogDialog
{
public:
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *tab;
    QTextBrowser *logView;
    QWidget *tab_2;
    QTextBrowser *errorView;
    QWidget *tab_3;
    QTextBrowser *warningView;

    void setupUi(QDialog *LogDialog)
    {
        if (LogDialog->objectName().isEmpty())
            LogDialog->setObjectName(QString::fromUtf8("LogDialog"));
        LogDialog->resize(664, 377);
        buttonBox = new QDialogButtonBox(LogDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(310, 340, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        tabWidget = new QTabWidget(LogDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 641, 321));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        logView = new QTextBrowser(tab);
        logView->setObjectName(QString::fromUtf8("logView"));
        logView->setGeometry(QRect(10, 10, 621, 271));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        errorView = new QTextBrowser(tab_2);
        errorView->setObjectName(QString::fromUtf8("errorView"));
        errorView->setGeometry(QRect(10, 10, 621, 271));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        warningView = new QTextBrowser(tab_3);
        warningView->setObjectName(QString::fromUtf8("warningView"));
        warningView->setGeometry(QRect(10, 10, 621, 271));
        tabWidget->addTab(tab_3, QString());

        retranslateUi(LogDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), LogDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), LogDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LogDialog);
    } // setupUi

    void retranslateUi(QDialog *LogDialog)
    {
        LogDialog->setWindowTitle(QApplication::translate("LogDialog", "Log Window", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("LogDialog", "Log", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("LogDialog", "Errors", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("LogDialog", "Warnings", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LogDialog: public Ui_LogDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGDIALOG_H
