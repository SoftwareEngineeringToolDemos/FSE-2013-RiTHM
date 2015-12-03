/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sun Jun 2 20:53:23 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEditSrcDir;
    QPushButton *pushButtonBrowseSrc;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *lineEditOutputDir;
    QPushButton *pushButtonBrowseOutput;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEditPropPath;
    QPushButton *pushButtonBrowseProp;
    QFrame *line_3;
    QTabWidget *tabWidget;
    QWidget *tab_4;
    QRadioButton *radioButtonFixed;
    QRadioButton *radioButtonDynamic;
    QFrame *frameFixedPolling;
    QWidget *layoutWidget_2;
    QFormLayout *formLayout_3;
    QLabel *label_14;
    QComboBox *comboBoxAlgType;
    QLabel *label_15;
    QComboBox *comboBoxInvocType;
    QLabel *label_16;
    QLineEdit *lineEditSP;
    QLabel *label_17;
    QLineEdit *lineEditBufSz;
    QWidget *layoutWidget_3;
    QFormLayout *formLayout_4;
    QComboBox *comboBoxExt;
    QLabel *label_4;
    QFrame *line_7;
    QFrame *frameDynamicPolling;
    QFrame *line_8;
    QFrame *line_9;
    QWidget *horizontalLayoutWidget_4;
    QHBoxLayout *horizontalLayout_11;
    QLabel *labelFuzzyScale;
    QLineEdit *lineEditFuzzyScale;
    QLabel *labelCV;
    QLineEdit *lineEditCV;
    QWidget *horizontalLayoutWidget_5;
    QHBoxLayout *horizontalLayout_12;
    QLabel *labelInvocFreq;
    QLineEdit *lineEditInvocFreq;
    QLabel *labelMaxBS;
    QLineEdit *lineEditMaxBS;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_3;
    QLabel *label_23;
    QLabel *labelPP;
    QLabel *labelMaxPP;
    QLineEdit *lineEditMaxPP;
    QComboBox *comboBoxDynamicMode;
    QLabel *label_24;
    QLineEdit *lineEditPP;
    QComboBox *comboBoxContType;
    QLabel *labelStaticBS;
    QLineEdit *lineEditStaticBS;
    QLabel *labelStaticBS_2;
    QLineEdit *lineEditSafety;
    QFrame *line_10;
    QWidget *horizontalLayoutWidget_6;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_25;
    QLineEdit *lineEditP;
    QLabel *label_26;
    QLineEdit *lineEditI;
    QLabel *label_27;
    QLineEdit *lineEditD;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *checkBoxLog;
    QPushButton *pushButtonRun;
    QPushButton *pushButtonViewLog;
    QWidget *tab_3;
    QLabel *label_13;
    QPushButton *pushButtonBeginPlot;
    QFrame *line;
    QLabel *labelStatus;
    QMenuBar *menuBar;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(597, 805);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        lineEditSrcDir = new QLineEdit(centralWidget);
        lineEditSrcDir->setObjectName(QString::fromUtf8("lineEditSrcDir"));

        horizontalLayout->addWidget(lineEditSrcDir);

        pushButtonBrowseSrc = new QPushButton(centralWidget);
        pushButtonBrowseSrc->setObjectName(QString::fromUtf8("pushButtonBrowseSrc"));

        horizontalLayout->addWidget(pushButtonBrowseSrc);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        lineEditOutputDir = new QLineEdit(centralWidget);
        lineEditOutputDir->setObjectName(QString::fromUtf8("lineEditOutputDir"));
        sizePolicy.setHeightForWidth(lineEditOutputDir->sizePolicy().hasHeightForWidth());
        lineEditOutputDir->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(lineEditOutputDir);

        pushButtonBrowseOutput = new QPushButton(centralWidget);
        pushButtonBrowseOutput->setObjectName(QString::fromUtf8("pushButtonBrowseOutput"));

        horizontalLayout_3->addWidget(pushButtonBrowseOutput);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEditPropPath = new QLineEdit(centralWidget);
        lineEditPropPath->setObjectName(QString::fromUtf8("lineEditPropPath"));

        horizontalLayout_2->addWidget(lineEditPropPath);

        pushButtonBrowseProp = new QPushButton(centralWidget);
        pushButtonBrowseProp->setObjectName(QString::fromUtf8("pushButtonBrowseProp"));

        horizontalLayout_2->addWidget(pushButtonBrowseProp);


        verticalLayout->addLayout(horizontalLayout_2);

        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_3);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        radioButtonFixed = new QRadioButton(tab_4);
        radioButtonFixed->setObjectName(QString::fromUtf8("radioButtonFixed"));
        radioButtonFixed->setGeometry(QRect(110, 10, 116, 22));
        radioButtonFixed->setChecked(true);
        radioButtonDynamic = new QRadioButton(tab_4);
        radioButtonDynamic->setObjectName(QString::fromUtf8("radioButtonDynamic"));
        radioButtonDynamic->setGeometry(QRect(330, 10, 131, 22));
        frameFixedPolling = new QFrame(tab_4);
        frameFixedPolling->setObjectName(QString::fromUtf8("frameFixedPolling"));
        frameFixedPolling->setGeometry(QRect(0, 39, 571, 281));
        frameFixedPolling->setFrameShape(QFrame::StyledPanel);
        frameFixedPolling->setFrameShadow(QFrame::Raised);
        layoutWidget_2 = new QWidget(frameFixedPolling);
        layoutWidget_2->setObjectName(QString::fromUtf8("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(0, 10, 571, 131));
        formLayout_3 = new QFormLayout(layoutWidget_2);
        formLayout_3->setSpacing(6);
        formLayout_3->setContentsMargins(11, 11, 11, 11);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        formLayout_3->setContentsMargins(0, 0, 0, 0);
        label_14 = new QLabel(layoutWidget_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_14->sizePolicy().hasHeightForWidth());
        label_14->setSizePolicy(sizePolicy1);

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_14);

        comboBoxAlgType = new QComboBox(layoutWidget_2);
        comboBoxAlgType->setObjectName(QString::fromUtf8("comboBoxAlgType"));
        sizePolicy.setHeightForWidth(comboBoxAlgType->sizePolicy().hasHeightForWidth());
        comboBoxAlgType->setSizePolicy(sizePolicy);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, comboBoxAlgType);

        label_15 = new QLabel(layoutWidget_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        sizePolicy1.setHeightForWidth(label_15->sizePolicy().hasHeightForWidth());
        label_15->setSizePolicy(sizePolicy1);

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_15);

        comboBoxInvocType = new QComboBox(layoutWidget_2);
        comboBoxInvocType->setObjectName(QString::fromUtf8("comboBoxInvocType"));
        sizePolicy.setHeightForWidth(comboBoxInvocType->sizePolicy().hasHeightForWidth());
        comboBoxInvocType->setSizePolicy(sizePolicy);

        formLayout_3->setWidget(1, QFormLayout::FieldRole, comboBoxInvocType);

        label_16 = new QLabel(layoutWidget_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_16->sizePolicy().hasHeightForWidth());
        label_16->setSizePolicy(sizePolicy2);

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_16);

        lineEditSP = new QLineEdit(layoutWidget_2);
        lineEditSP->setObjectName(QString::fromUtf8("lineEditSP"));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, lineEditSP);

        label_17 = new QLabel(layoutWidget_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label_17->sizePolicy().hasHeightForWidth());
        label_17->setSizePolicy(sizePolicy3);

        formLayout_3->setWidget(3, QFormLayout::LabelRole, label_17);

        lineEditBufSz = new QLineEdit(layoutWidget_2);
        lineEditBufSz->setObjectName(QString::fromUtf8("lineEditBufSz"));

        formLayout_3->setWidget(3, QFormLayout::FieldRole, lineEditBufSz);

        layoutWidget_3 = new QWidget(frameFixedPolling);
        layoutWidget_3->setObjectName(QString::fromUtf8("layoutWidget_3"));
        layoutWidget_3->setGeometry(QRect(0, 160, 571, 62));
        formLayout_4 = new QFormLayout(layoutWidget_3);
        formLayout_4->setSpacing(6);
        formLayout_4->setContentsMargins(11, 11, 11, 11);
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        formLayout_4->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout_4->setContentsMargins(0, 0, 0, 0);
        comboBoxExt = new QComboBox(layoutWidget_3);
        comboBoxExt->setObjectName(QString::fromUtf8("comboBoxExt"));
        sizePolicy.setHeightForWidth(comboBoxExt->sizePolicy().hasHeightForWidth());
        comboBoxExt->setSizePolicy(sizePolicy);

        formLayout_4->setWidget(0, QFormLayout::FieldRole, comboBoxExt);

        label_4 = new QLabel(layoutWidget_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_4);

        line_7 = new QFrame(frameFixedPolling);
        line_7->setObjectName(QString::fromUtf8("line_7"));
        line_7->setGeometry(QRect(0, 150, 579, 3));
        line_7->setFrameShape(QFrame::HLine);
        line_7->setFrameShadow(QFrame::Sunken);
        frameDynamicPolling = new QFrame(tab_4);
        frameDynamicPolling->setObjectName(QString::fromUtf8("frameDynamicPolling"));
        frameDynamicPolling->setGeometry(QRect(0, 40, 571, 321));
        frameDynamicPolling->setAutoFillBackground(true);
        frameDynamicPolling->setFrameShape(QFrame::StyledPanel);
        frameDynamicPolling->setFrameShadow(QFrame::Raised);
        line_8 = new QFrame(frameDynamicPolling);
        line_8->setObjectName(QString::fromUtf8("line_8"));
        line_8->setGeometry(QRect(0, 260, 579, 3));
        line_8->setFrameShape(QFrame::HLine);
        line_8->setFrameShadow(QFrame::Sunken);
        line_9 = new QFrame(frameDynamicPolling);
        line_9->setObjectName(QString::fromUtf8("line_9"));
        line_9->setGeometry(QRect(0, 200, 579, 3));
        line_9->setFrameShape(QFrame::HLine);
        line_9->setFrameShadow(QFrame::Sunken);
        horizontalLayoutWidget_4 = new QWidget(frameDynamicPolling);
        horizontalLayoutWidget_4->setObjectName(QString::fromUtf8("horizontalLayoutWidget_4"));
        horizontalLayoutWidget_4->setGeometry(QRect(0, 150, 571, 41));
        horizontalLayout_11 = new QHBoxLayout(horizontalLayoutWidget_4);
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        horizontalLayout_11->setContentsMargins(0, 0, 0, 0);
        labelFuzzyScale = new QLabel(horizontalLayoutWidget_4);
        labelFuzzyScale->setObjectName(QString::fromUtf8("labelFuzzyScale"));
        sizePolicy2.setHeightForWidth(labelFuzzyScale->sizePolicy().hasHeightForWidth());
        labelFuzzyScale->setSizePolicy(sizePolicy2);

        horizontalLayout_11->addWidget(labelFuzzyScale);

        lineEditFuzzyScale = new QLineEdit(horizontalLayoutWidget_4);
        lineEditFuzzyScale->setObjectName(QString::fromUtf8("lineEditFuzzyScale"));

        horizontalLayout_11->addWidget(lineEditFuzzyScale);

        labelCV = new QLabel(horizontalLayoutWidget_4);
        labelCV->setObjectName(QString::fromUtf8("labelCV"));
        sizePolicy2.setHeightForWidth(labelCV->sizePolicy().hasHeightForWidth());
        labelCV->setSizePolicy(sizePolicy2);

        horizontalLayout_11->addWidget(labelCV);

        lineEditCV = new QLineEdit(horizontalLayoutWidget_4);
        lineEditCV->setObjectName(QString::fromUtf8("lineEditCV"));

        horizontalLayout_11->addWidget(lineEditCV);

        horizontalLayoutWidget_5 = new QWidget(frameDynamicPolling);
        horizontalLayoutWidget_5->setObjectName(QString::fromUtf8("horizontalLayoutWidget_5"));
        horizontalLayoutWidget_5->setGeometry(QRect(0, 270, 571, 41));
        horizontalLayout_12 = new QHBoxLayout(horizontalLayoutWidget_5);
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        horizontalLayout_12->setContentsMargins(0, 0, 0, 0);
        labelInvocFreq = new QLabel(horizontalLayoutWidget_5);
        labelInvocFreq->setObjectName(QString::fromUtf8("labelInvocFreq"));
        sizePolicy2.setHeightForWidth(labelInvocFreq->sizePolicy().hasHeightForWidth());
        labelInvocFreq->setSizePolicy(sizePolicy2);

        horizontalLayout_12->addWidget(labelInvocFreq);

        lineEditInvocFreq = new QLineEdit(horizontalLayoutWidget_5);
        lineEditInvocFreq->setObjectName(QString::fromUtf8("lineEditInvocFreq"));

        horizontalLayout_12->addWidget(lineEditInvocFreq);

        labelMaxBS = new QLabel(horizontalLayoutWidget_5);
        labelMaxBS->setObjectName(QString::fromUtf8("labelMaxBS"));
        sizePolicy3.setHeightForWidth(labelMaxBS->sizePolicy().hasHeightForWidth());
        labelMaxBS->setSizePolicy(sizePolicy3);

        horizontalLayout_12->addWidget(labelMaxBS);

        lineEditMaxBS = new QLineEdit(horizontalLayoutWidget_5);
        lineEditMaxBS->setObjectName(QString::fromUtf8("lineEditMaxBS"));

        horizontalLayout_12->addWidget(lineEditMaxBS);

        gridLayoutWidget_2 = new QWidget(frameDynamicPolling);
        gridLayoutWidget_2->setObjectName(QString::fromUtf8("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(0, 0, 571, 131));
        gridLayout_3 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(0);
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        label_23 = new QLabel(gridLayoutWidget_2);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        sizePolicy1.setHeightForWidth(label_23->sizePolicy().hasHeightForWidth());
        label_23->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(label_23, 1, 0, 1, 1);

        labelPP = new QLabel(gridLayoutWidget_2);
        labelPP->setObjectName(QString::fromUtf8("labelPP"));
        sizePolicy2.setHeightForWidth(labelPP->sizePolicy().hasHeightForWidth());
        labelPP->setSizePolicy(sizePolicy2);

        gridLayout_3->addWidget(labelPP, 2, 0, 1, 1);

        labelMaxPP = new QLabel(gridLayoutWidget_2);
        labelMaxPP->setObjectName(QString::fromUtf8("labelMaxPP"));
        sizePolicy2.setHeightForWidth(labelMaxPP->sizePolicy().hasHeightForWidth());
        labelMaxPP->setSizePolicy(sizePolicy2);

        gridLayout_3->addWidget(labelMaxPP, 2, 3, 1, 1);

        lineEditMaxPP = new QLineEdit(gridLayoutWidget_2);
        lineEditMaxPP->setObjectName(QString::fromUtf8("lineEditMaxPP"));

        gridLayout_3->addWidget(lineEditMaxPP, 2, 4, 1, 1);

        comboBoxDynamicMode = new QComboBox(gridLayoutWidget_2);
        comboBoxDynamicMode->setObjectName(QString::fromUtf8("comboBoxDynamicMode"));
        sizePolicy.setHeightForWidth(comboBoxDynamicMode->sizePolicy().hasHeightForWidth());
        comboBoxDynamicMode->setSizePolicy(sizePolicy);

        gridLayout_3->addWidget(comboBoxDynamicMode, 1, 2, 1, 3);

        label_24 = new QLabel(gridLayoutWidget_2);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        sizePolicy1.setHeightForWidth(label_24->sizePolicy().hasHeightForWidth());
        label_24->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(label_24, 0, 0, 1, 1);

        lineEditPP = new QLineEdit(gridLayoutWidget_2);
        lineEditPP->setObjectName(QString::fromUtf8("lineEditPP"));

        gridLayout_3->addWidget(lineEditPP, 2, 2, 1, 1);

        comboBoxContType = new QComboBox(gridLayoutWidget_2);
        comboBoxContType->setObjectName(QString::fromUtf8("comboBoxContType"));
        sizePolicy.setHeightForWidth(comboBoxContType->sizePolicy().hasHeightForWidth());
        comboBoxContType->setSizePolicy(sizePolicy);

        gridLayout_3->addWidget(comboBoxContType, 0, 2, 1, 3);

        labelStaticBS = new QLabel(gridLayoutWidget_2);
        labelStaticBS->setObjectName(QString::fromUtf8("labelStaticBS"));
        sizePolicy3.setHeightForWidth(labelStaticBS->sizePolicy().hasHeightForWidth());
        labelStaticBS->setSizePolicy(sizePolicy3);

        gridLayout_3->addWidget(labelStaticBS, 3, 0, 1, 1);

        lineEditStaticBS = new QLineEdit(gridLayoutWidget_2);
        lineEditStaticBS->setObjectName(QString::fromUtf8("lineEditStaticBS"));

        gridLayout_3->addWidget(lineEditStaticBS, 3, 2, 1, 1);

        labelStaticBS_2 = new QLabel(gridLayoutWidget_2);
        labelStaticBS_2->setObjectName(QString::fromUtf8("labelStaticBS_2"));
        sizePolicy3.setHeightForWidth(labelStaticBS_2->sizePolicy().hasHeightForWidth());
        labelStaticBS_2->setSizePolicy(sizePolicy3);

        gridLayout_3->addWidget(labelStaticBS_2, 3, 3, 1, 1);

        lineEditSafety = new QLineEdit(gridLayoutWidget_2);
        lineEditSafety->setObjectName(QString::fromUtf8("lineEditSafety"));

        gridLayout_3->addWidget(lineEditSafety, 3, 4, 1, 1);

        line_10 = new QFrame(frameDynamicPolling);
        line_10->setObjectName(QString::fromUtf8("line_10"));
        line_10->setGeometry(QRect(0, 140, 579, 3));
        line_10->setFrameShape(QFrame::HLine);
        line_10->setFrameShadow(QFrame::Sunken);
        horizontalLayoutWidget_6 = new QWidget(frameDynamicPolling);
        horizontalLayoutWidget_6->setObjectName(QString::fromUtf8("horizontalLayoutWidget_6"));
        horizontalLayoutWidget_6->setGeometry(QRect(0, 210, 571, 41));
        horizontalLayout_13 = new QHBoxLayout(horizontalLayoutWidget_6);
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        horizontalLayout_13->setContentsMargins(0, 0, 0, 0);
        label_25 = new QLabel(horizontalLayoutWidget_6);
        label_25->setObjectName(QString::fromUtf8("label_25"));

        horizontalLayout_13->addWidget(label_25);

        lineEditP = new QLineEdit(horizontalLayoutWidget_6);
        lineEditP->setObjectName(QString::fromUtf8("lineEditP"));

        horizontalLayout_13->addWidget(lineEditP);

        label_26 = new QLabel(horizontalLayoutWidget_6);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        horizontalLayout_13->addWidget(label_26);

        lineEditI = new QLineEdit(horizontalLayoutWidget_6);
        lineEditI->setObjectName(QString::fromUtf8("lineEditI"));

        horizontalLayout_13->addWidget(lineEditI);

        label_27 = new QLabel(horizontalLayoutWidget_6);
        label_27->setObjectName(QString::fromUtf8("label_27"));

        horizontalLayout_13->addWidget(label_27);

        lineEditD = new QLineEdit(horizontalLayoutWidget_6);
        lineEditD->setObjectName(QString::fromUtf8("lineEditD"));

        horizontalLayout_13->addWidget(lineEditD);

        layoutWidget = new QWidget(tab_4);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 370, 571, 91));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        checkBoxLog = new QCheckBox(layoutWidget);
        checkBoxLog->setObjectName(QString::fromUtf8("checkBoxLog"));

        horizontalLayout_5->addWidget(checkBoxLog);

        pushButtonRun = new QPushButton(layoutWidget);
        pushButtonRun->setObjectName(QString::fromUtf8("pushButtonRun"));
        pushButtonRun->setEnabled(true);

        horizontalLayout_5->addWidget(pushButtonRun);

        pushButtonViewLog = new QPushButton(layoutWidget);
        pushButtonViewLog->setObjectName(QString::fromUtf8("pushButtonViewLog"));
        pushButtonViewLog->setEnabled(true);

        horizontalLayout_5->addWidget(pushButtonViewLog);

        tabWidget->addTab(tab_4, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        label_13 = new QLabel(tab_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(140, 100, 281, 51));
        label_13->setAlignment(Qt::AlignCenter);
        pushButtonBeginPlot = new QPushButton(tab_3);
        pushButtonBeginPlot->setObjectName(QString::fromUtf8("pushButtonBeginPlot"));
        pushButtonBeginPlot->setGeometry(QRect(230, 150, 98, 27));
        tabWidget->addTab(tab_3, QString());

        verticalLayout->addWidget(tabWidget);

        line = new QFrame(centralWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        labelStatus = new QLabel(centralWidget);
        labelStatus->setObjectName(QString::fromUtf8("labelStatus"));
        sizePolicy1.setHeightForWidth(labelStatus->sizePolicy().hasHeightForWidth());
        labelStatus->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(labelStatus);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 597, 25));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(menuBar->sizePolicy().hasHeightForWidth());
        menuBar->setSizePolicy(sizePolicy4);
        menuBar->setDefaultUp(false);
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        QWidget::setTabOrder(lineEditSrcDir, pushButtonBrowseSrc);
        QWidget::setTabOrder(pushButtonBrowseSrc, lineEditPropPath);
        QWidget::setTabOrder(lineEditPropPath, pushButtonBrowseProp);

        retranslateUi(MainWindow);
        QObject::connect(pushButtonBrowseSrc, SIGNAL(clicked()), MainWindow, SLOT(BrowseSrcDir()));
        QObject::connect(pushButtonBrowseProp, SIGNAL(clicked()), MainWindow, SLOT(BrowsePropFilepath()));
        QObject::connect(pushButtonBrowseOutput, SIGNAL(clicked()), MainWindow, SLOT(BrowseOutputDir()));
        QObject::connect(pushButtonBeginPlot, SIGNAL(clicked()), MainWindow, SLOT(on_pushButton_clicked()));
        QObject::connect(radioButtonDynamic, SIGNAL(toggled(bool)), MainWindow, SLOT(DynamicToggled(bool)));
        QObject::connect(radioButtonFixed, SIGNAL(toggled(bool)), MainWindow, SLOT(FixedToggled(bool)));
        QObject::connect(pushButtonRun, SIGNAL(clicked()), MainWindow, SLOT(RunTool()));
        QObject::connect(comboBoxContType, SIGNAL(currentIndexChanged(QString)), MainWindow, SLOT(ControllerChanged(QString)));
        QObject::connect(comboBoxDynamicMode, SIGNAL(currentIndexChanged(int)), MainWindow, SLOT(DynamicChanged(int)));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "RiTHM", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Source Directory:  ", 0, QApplication::UnicodeUTF8));
        pushButtonBrowseSrc->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Output Directory: ", 0, QApplication::UnicodeUTF8));
        pushButtonBrowseOutput->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Property Filepath:", 0, QApplication::UnicodeUTF8));
        pushButtonBrowseProp->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
        radioButtonFixed->setText(QApplication::translate("MainWindow", "Fixed Polling", 0, QApplication::UnicodeUTF8));
        radioButtonDynamic->setText(QApplication::translate("MainWindow", "Dynamic Polling", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("MainWindow", "Algorithm Type: ", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MainWindow", "Invocation Type: ", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("MainWindow", "Target Polling Period [cycles]: ", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MainWindow", "Buffer Size [bytes]: ", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "External", 0, QApplication::UnicodeUTF8));
        labelFuzzyScale->setText(QApplication::translate("MainWindow", "Fuzzy Scaling Factor:", 0, QApplication::UnicodeUTF8));
        labelCV->setText(QApplication::translate("MainWindow", "Target CV [0-1]: ", 0, QApplication::UnicodeUTF8));
        labelInvocFreq->setText(QApplication::translate("MainWindow", "Controller Invocation Frequency:", 0, QApplication::UnicodeUTF8));
        labelMaxBS->setText(QApplication::translate("MainWindow", "Maximum Buffer Size [events]: ", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("MainWindow", "Dynamic Buffer:", 0, QApplication::UnicodeUTF8));
        labelPP->setText(QApplication::translate("MainWindow", "Initial Polling Period [msec]: ", 0, QApplication::UnicodeUTF8));
        labelMaxPP->setText(QApplication::translate("MainWindow", "Maximum Polling Period [msec]: ", 0, QApplication::UnicodeUTF8));
        label_24->setText(QApplication::translate("MainWindow", "Controller Type: ", 0, QApplication::UnicodeUTF8));
        labelStaticBS->setText(QApplication::translate("MainWindow", "Static Buffer Size [events]: ", 0, QApplication::UnicodeUTF8));
        labelStaticBS_2->setText(QApplication::translate("MainWindow", "Safety Percentage [0-100]: ", 0, QApplication::UnicodeUTF8));
        label_25->setText(QApplication::translate("MainWindow", "P", 0, QApplication::UnicodeUTF8));
        label_26->setText(QApplication::translate("MainWindow", "I", 0, QApplication::UnicodeUTF8));
        label_27->setText(QApplication::translate("MainWindow", "D", 0, QApplication::UnicodeUTF8));
        checkBoxLog->setText(QApplication::translate("MainWindow", "Log Trace - file rithm_trace.log", 0, QApplication::UnicodeUTF8));
        pushButtonRun->setText(QApplication::translate("MainWindow", "Run Tool", 0, QApplication::UnicodeUTF8));
        pushButtonViewLog->setText(QApplication::translate("MainWindow", "View Log", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MainWindow", "Configuration", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("MainWindow", "Monitoring /proc/rithm", 0, QApplication::UnicodeUTF8));
        pushButtonBeginPlot->setText(QApplication::translate("MainWindow", "Begin", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Realtime Plot", 0, QApplication::UnicodeUTF8));
        labelStatus->setText(QApplication::translate("MainWindow", "Press run on selection, Press 'View-Log', when the instrumented code's object is run", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
