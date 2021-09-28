#include "LogView.h"
#include "log/Log.h"

#include "ui_LogView.h"

LogView::LogView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogView)
{
    ui->setupUi(this);

    connect(&Log::instance(), SIGNAL(message(const QString&)), this, SLOT(message(const QString&)));
}

LogView::~LogView()
{
    delete ui;
}

void LogView::message(const QString& msg)
{
    QString logText = ui->plainTextEdit->toPlainText();

    QStringList logList = logText.split("\r\n");
    if(logList.size()>100)
    {
        logList.removeLast();
    }

    logList.push_front(msg);

    ui->plainTextEdit->setPlainText(logList.join("\r\n"));
}
