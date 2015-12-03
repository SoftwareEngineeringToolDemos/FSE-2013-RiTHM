/********************************************************************************
** Form generated from reading UI file 'settingswindow.ui'
**
** Created: Sun May 26 17:10:19 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSWINDOW_H
#define UI_SETTINGSWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_SettingsWindow
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingsWindow)
    {
        if (SettingsWindow->objectName().isEmpty())
            SettingsWindow->setObjectName(QString::fromUtf8("SettingsWindow"));
        SettingsWindow->resize(400, 300);
        buttonBox = new QDialogButtonBox(SettingsWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(SettingsWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingsWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(SettingsWindow);
    } // setupUi

    void retranslateUi(QDialog *SettingsWindow)
    {
        SettingsWindow->setWindowTitle(QApplication::translate("SettingsWindow", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SettingsWindow: public Ui_SettingsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSWINDOW_H
