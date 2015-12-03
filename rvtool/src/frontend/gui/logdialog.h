#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LogDialog(QWidget *parent = 0);
    QString command;
    void Begin();
    ~LogDialog();

private slots:
    void updateLog();

private:
    Ui::LogDialog *ui;
    FILE *stream;
};

#endif // LOGDIALOG_H
