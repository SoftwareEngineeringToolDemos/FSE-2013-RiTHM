#include "logdialog.h"
#include "ui_logdialog.h"
#include <QTimer>
#include <QTextStream>
#include <QDebug>

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);


}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::Begin() {
    stream = popen(command.toLocal8Bit().data(), "r");

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateLog()));
    timer->start(1);
}

void LogDialog::updateLog()
{
    char buffer[255];
    if (fgets(buffer, 255, stream) != NULL) {
        QString line = QString(buffer);
        while(line.endsWith('\n')) line.chop(1);
        if (line.indexOf("error",0,Qt::CaseInsensitive)>=0) {
            ui->logView->append("<font color=\"#ff0000\">" + line + "</font>");
            ui->errorView->append("<font color=\"#ff0000\">" + line + "</font>");
        }
        else  if (line.indexOf("warn",0,Qt::CaseInsensitive)>=0) {
            ui->logView->append("<font color=\"#0000ff\">" + line + "</font>");
            ui->warningView->append("<font color=\"#0000ff\">" + line + "</font>");
        }
        else {
            ui->logView->append(line);
        }
    }
}
