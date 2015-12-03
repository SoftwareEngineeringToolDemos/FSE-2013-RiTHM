#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"
#include "logdialog.h"
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QStringList algTypes, invocationTypes, externalSchemes, selfSchemes, controllers, dynamicToggle;
    algTypes << "Sequential" << "Partial Offload" << "Finite History" << "Infinite History";
    invocationTypes << "Synchronous" << "Asynchronous";
    externalSchemes << "ILP" << "H1 (Greedy)" << "H2 (Approx. Min. Vertex Cover)";
    selfSchemes << "SAT" << "Heuristic";
    controllers << "PID" << "Fuzzy 1" << "Fuzzy 2" << "Fuzzy 3" << "Fuzzy 3 - Remapped";
    dynamicToggle << "Enabled" << "Disabled";

    ui->setupUi(this);

    plot = new MonitorPlot( ui->tab_3 );
    plot->setTitle( "History" );

    const int margin = 5;
    plot->setContentsMargins( margin, margin, margin, margin );

    QVBoxLayout *layout = new QVBoxLayout( ui->tab_3 );
    layout->addWidget(plot);

    plot->setHidden(TRUE);

    ui->frameDynamicPolling->setHidden(TRUE);

    ui->comboBoxAlgType->addItems(algTypes);
    ui->comboBoxInvocType->addItems(invocationTypes);
    ui->comboBoxExt->addItems(externalSchemes);
    ui->comboBoxContType->addItems(controllers);
    ui->comboBoxDynamicMode->addItems(dynamicToggle);

    about = new QAction("&About", this);
    settings = new QAction("&Settings", this);
    run = new QAction("&Run", this);
    QMenu *menu = menuBar()->addMenu("&RiTHM");
    menu->addAction("&About");
    menu->addAction("&Settings");
    menu->addAction("&Run");
}

MainWindow::~MainWindow()
{
    delete ui;
}

const QString MainWindow::SolveTypeArg[] = {"ilp", "heu1", "heu2", "sat", "greedy"};
const QString MainWindow::GooMFAlgType[] = {"_GOOMF_enum_alg_seq", "_GOOMF_enum_alg_partial_offload",
                                            "_GOOMF_enum_alg_finite", "_GOOMF_enum_alg_infinite"};
const QString MainWindow::GooMFInvokeType[] = {"_GOOMF_enum_sync_invocation", "_GOOMF_enum_async_invocation"};

void MainWindow::LaunchSettingsWindow() {
    SettingsWindow *settings = new SettingsWindow;
    settings->setModal(true);
    settings->show();
}

void MainWindow::BrowseSrcDir()
{
    QString mString = QFileDialog::getExistingDirectory(this,
         tr("Select Source Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->lineEditSrcDir->insert(mString);

}

void MainWindow::BrowseOutputDir()
{
    QString mString = QFileDialog::getExistingDirectory(this,
         tr("Select Output Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->lineEditOutputDir->insert(mString);
}

void MainWindow::BrowsePropFilepath()
{
    QString mString = QFileDialog::getOpenFileName(this,
         tr("Select Property File"), "/home", tr("Property file (*.cfg)"));
    ui->lineEditPropPath->insert(mString);
}

void MainWindow::RunTool()
{
    QString srcDir = ui->lineEditSrcDir->text();
    QString outDir = ui->lineEditOutputDir->text();
    QString propPath = ui->lineEditPropPath->text();

    if (srcDir.isEmpty() || !QDir(srcDir).exists())
    {
        ui->labelStatus->setText("Valid source directory must be specified");
        return;
    }

    if (outDir.isEmpty() || !QDir(outDir).exists())
    {
        system((QString("mkdir ") + outDir).toLocal8Bit().data());
    }

    if (propPath.isEmpty() || !QFile(propPath).exists())
    {
        ui->labelStatus->setText("Valid filepath to properties file must be specified");
        return;
    }

    // File modifications
    QFile controllerFile("./monitor/Controller.h");
    QFile fuzzyFile("./monitor/fuzzy.h");
    QFile pidFile("./monitor/pid.h");
    QStringList strings;

    QFile controllerOut(outDir + "/Controller.h");
    QFile fuzzyOut(outDir + "/fuzzy.h");
    QFile pidOut(outDir + "/pid.h");

    if (ui->radioButtonDynamic->isChecked())
    {
        bool convertOk;
        int value = ui->lineEditSafety->text().toInt(&convertOk);
        if (!convertOk || value < 0 || value > 100)
        {
            ui->labelStatus->setText("Safety must be between 0 and 100");
            return;
        }

        value = ui->lineEditStaticBS->text().toInt(&convertOk);
        if (!convertOk || value <= 0)
        {
            ui->labelStatus->setText("Static buffer size must be a positive integer");
            return;
        }

        if (ui->comboBoxDynamicMode->currentIndex() == 0) {
            value = ui->lineEditMaxBS->text().toInt(&convertOk);
            if (!convertOk || value <= 0)
            {
                ui->labelStatus->setText("Max buffer size must be a positive integer");
                return;
            }

            value = ui->lineEditInvocFreq->text().toInt(&convertOk);
            if (!convertOk || value <= 0)
            {
                ui->labelStatus->setText("Invocation frequency must be a positive integer");
                return;
            }
        }

        if (ui->comboBoxContType->currentIndex()+1>=2) {
            value = ui->lineEditFuzzyScale->text().toInt(&convertOk);
            if (!convertOk || value <= 0)
            {
                ui->labelStatus->setText("Fuzzy scale must be a positive integer");
                return;
            }
        }

        if (ui->comboBoxContType->currentIndex()+1>=4) {
            double dvalue = ui->lineEditCV->text().toDouble(&convertOk);
            if (!convertOk || dvalue < 0 || dvalue > 1.0)
            {
                ui->labelStatus->setText("Coefficient of variation must be a number between 0 and 1");
                return;
            }
        }

        if (ui->comboBoxContType->currentIndex()+1==1) {
            value = ui->lineEditP->text().toInt(&convertOk);
            if (!convertOk || value <= 0)
            {
                ui->labelStatus->setText("P value must be a positive integer");
                return;
            }
            value = ui->lineEditI->text().toInt(&convertOk);
            if (!convertOk || value <= 0)
            {
                ui->labelStatus->setText("I value must be a positive integer");
                return;
            }
            value = ui->lineEditD->text().toInt(&convertOk);
            if (!convertOk || value <= 0)
            {
                ui->labelStatus->setText("D value must be a positive integer");
                return;
            }
        }

        value = ui->lineEditPP->text().toInt(&convertOk);
        if (!convertOk || value <= 0)
        {
            ui->labelStatus->setText("Initial polling period must be a positive integer");
            return;
        }

        value = ui->lineEditMaxPP->text().toInt(&convertOk);
        if (!convertOk || value <= 0)
        {
            ui->labelStatus->setText("Max polling period must be a positive integer");
            return;
        }

        controllerFile.open(QFile::ReadOnly | QFile::Text);
        controllerOut.open(QFile::ReadWrite | QFile::Text);
        QTextStream in(&controllerFile);
        QTextStream out(&controllerOut);
        while(!in.atEnd()) {
            QString line = in.readLine();
            if (line.contains("__CONTROLLERTYPE__")) {
                QString controller = QString::number(ui->comboBoxContType->currentIndex()+1);
                line.replace("__CONTROLLERTYPE__",controller);
            }
            else if (line.contains("__SAFETY__")) {
                line.replace("__SAFETY__",ui->lineEditSafety->text());
            }
            else if (line.contains("__CV__")) {
                if (ui->comboBoxContType->currentIndex()+1>=4)
                    line.replace("__CV__",ui->lineEditCV->text());
                else
                    line.replace("__CV__","1.0");
            }
            else if (line.contains("__BS__")) {
                line.replace("__BS__",ui->lineEditStaticBS->text());
            }
            else if (line.contains("__INITIALPP__")) {
                line.replace("__INITIALPP__",ui->lineEditPP->text());
            }
            else if (line.contains("__MAXPP__")) {
                line.replace("__MAXPP__",ui->lineEditMaxPP->text());
            }
            else if (line.contains("__MAXBS__")) {
                if (ui->comboBoxDynamicMode->currentIndex() == 0)
                    line.replace("__MAXBS__",ui->lineEditMaxBS->text());
                else
                    line.replace("__MAXBS__","100");
            }
            else if (line.contains("__FREQ__")) {
                if (ui->comboBoxDynamicMode->currentIndex() == 0)
                    line.replace("__FREQ__",ui->lineEditInvocFreq->text());
                else
                    line.replace("__FREQ__","100");
            }
            out << line << endl;
        }

        fuzzyFile.open(QFile::ReadOnly | QFile::Text);
        fuzzyOut.open(QFile::ReadWrite | QFile::Text);
        QTextStream inF(&fuzzyFile);
        QTextStream outF(&fuzzyOut);
        while(!inF.atEnd()) {
            QString line = inF.readLine();
            if (line.contains("__CV__")) {
                if (ui->comboBoxContType->currentIndex()+1>=4)
                    line.replace("__CV__",ui->lineEditCV->text());
                else
                    line.replace("__CV__","1.0");
            }
            else if (line.contains("__SCALE__")) {
                if (ui->comboBoxContType->currentIndex()+1>=2)
                    line.replace("__SCALE__",ui->lineEditFuzzyScale->text());
                else
                    line.replace("__SCALE__","1");
            }
            outF << line << endl;
        }

        pidFile.open(QFile::ReadOnly | QFile::Text);
        pidOut.open(QFile::ReadWrite | QFile::Text);
        QTextStream inP(&pidFile);
        QTextStream outP(&pidOut);
        while(!inP.atEnd()) {
            QString line = inP.readLine();
            if (line.contains("__PID_P__")) {
                if (ui->comboBoxContType->currentIndex()+1==1)
                    line.replace("__PID_P__",ui->lineEditP->text());
                else
                    line.replace("__PID_P__","1.0");
            }
            else if (line.contains("__PID_I__")) {
                if (ui->comboBoxContType->currentIndex()+1==1)
                    line.replace("__PID_I__",ui->lineEditI->text());
                else
                    line.replace("__PID_I__","1.0");
            }
            else if (line.contains("__PID_D__")) {
                if (ui->comboBoxContType->currentIndex()+1==1)
                    line.replace("__PID_D__",ui->lineEditD->text());
                else
                    line.replace("__PID_D__","1.0");
            }
            outP << line << endl;
        }

        controllerOut.close();
        controllerFile.close();
        fuzzyFile.close();
        fuzzyOut.close();
        pidFile.close();
        pidOut.close();

        QString command = "./run.sh " + srcDir + " " + outDir + " " + propPath
                + " " + ((ui->checkBoxLog->isChecked()) ? "1" : "0") + " 2>&1" ;

        LogDialog *log = new LogDialog();
        log->command = command;
        log->show();
        log->Begin();

//        QString displaylog = "tail -f /proc/rithm_trace.log";
//        LogDialog *trace = new LogDialog();
//        trace->command = command;
//        trace->show();
//        trace->Begin();

    }
    else {

        QString spStr = ui->lineEditSP->text();
        QString bufSzStr = ui->lineEditBufSz->text();

        bool convertOk;
        int sp = spStr.toInt(&convertOk);
        if (!convertOk || sp <= 0)
        {
            ui->labelStatus->setText("Sampling period must be a positive integer");
            return;
        }

        int bufSz = bufSzStr.toInt(&convertOk);
        if (!convertOk || bufSz < 0)
        {
            ui->labelStatus->setText("Buffer size must be a positive integer");
            return;
        }

        QString solveMode;
        int mode = ui->comboBoxExt->currentIndex();
        switch(mode)
        {
        case 0:
            solveMode = SolveTypeArg[SOLVE_ILP];
            break;
        case 1:
            solveMode = SolveTypeArg[SOLVE_H1];
            break;
        case 2:
            solveMode = SolveTypeArg[SOLVE_H2];
            break;
        }

        QString command = "./run.sh " + srcDir + " " + outDir + " " + propPath
                + " " + solveMode + " " + spStr + " " + GooMFAlgType[ui->comboBoxAlgType->currentIndex()]
                + " " + GooMFInvokeType[ui->comboBoxInvocType->currentIndex()] + " " + bufSzStr + " " + ((ui->checkBoxLog->isChecked()) ? "1" : "0")  + " 2>&1";

        LogDialog *log = new LogDialog();
        log->command = command;
        log->show();
        log->Begin();

//        QString displaylog = "tail -f /proc/rithm_trace.log";
//        LogDialog *trace = new LogDialog();
//        trace->command = command;
//        trace->show();
//        trace->Begin();

    }
}

void MainWindow::ControllerChanged(const QString &arg1)
{
    if(arg1.compare("PID") == 0)
    {
        ui->lineEditP->setEnabled(TRUE);
        ui->lineEditI->setEnabled(TRUE);
        ui->lineEditD->setEnabled(TRUE);

        ui->lineEditFuzzyScale->setEnabled(FALSE);
        ui->lineEditCV->setEnabled(FALSE);
    }
    else {
        ui->lineEditP->setEnabled(FALSE);
        ui->lineEditI->setEnabled(FALSE);
        ui->lineEditD->setEnabled(FALSE);

        ui->lineEditFuzzyScale->setEnabled(TRUE);

        if(arg1.compare("Fuzzy 3") == 0 || arg1.compare("Fuzzy 3 - Remapped") == 0)
            ui->lineEditCV->setEnabled(TRUE);
        else
            ui->lineEditCV->setEnabled(FALSE);
    }
}

void MainWindow::DynamicChanged(int index)
{
    if (index == 0)
    {
        ui->lineEditInvocFreq->setEnabled(TRUE);
        ui->lineEditMaxBS->setEnabled(TRUE);
    }
    else {
        ui->lineEditInvocFreq->setEnabled(FALSE);
        ui->lineEditMaxBS->setEnabled(FALSE);
    }
}

void MainWindow::on_pushButton_clicked()
{
    plot->setHidden(FALSE);
}

void MainWindow::FixedToggled(bool checked)
{
    if (checked)
    {
        ui->radioButtonDynamic->setChecked(FALSE);
        ui->frameFixedPolling->setHidden(FALSE);
        ui->frameDynamicPolling->setHidden(TRUE);
    }
    else {
        ui->radioButtonDynamic->setChecked(TRUE);
        ui->frameFixedPolling->setHidden(TRUE);
        ui->frameDynamicPolling->setHidden(FALSE);
    }
}

void MainWindow::DynamicToggled(bool checked)
{
    if (checked)
    {
        ui->radioButtonFixed->setChecked(FALSE);
        ui->frameFixedPolling->setHidden(TRUE);
        ui->frameDynamicPolling->setHidden(FALSE);
    }
    else {
        ui->radioButtonFixed->setChecked(TRUE);
        ui->frameFixedPolling->setHidden(FALSE);
        ui->frameDynamicPolling->setHidden(TRUE);
    }
}

void MainWindow::on_pushButtonViewLog_clicked()
{
    QString outDir = ui->lineEditOutputDir->text();
    QString displaylog = "cat /tmp/rithm_trace.log";
    LogDialog *trace = new LogDialog();
    trace->command = displaylog;
    trace->show();
    trace->Begin();
}
