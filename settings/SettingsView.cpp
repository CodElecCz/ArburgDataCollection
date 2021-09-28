#include "SettingsView.h"
#include "Settings.h"

#include "ui_SettingsView.h"

#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>

SettingsView::SettingsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsView)
{
    ui->setupUi(this);

    openSettingsFile();

    ui->labelFilePath->setText(Settings::iniFilePath());
}

SettingsView::~SettingsView()
{
    delete ui;
}

void SettingsView::on_saveButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Save", "Save settings?\n\nRestart application after update settings.\n", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        saveSettingsFile();
    }
}

void SettingsView::on_cancelButton_clicked()
{
    openSettingsFile();
}

void SettingsView::openSettingsFile()
{
    QFile file(Settings::iniFilePath());
    if(!file.open(QIODevice::ReadOnly))
    {
        //error
        qDebug() << "Unable to open file: " << Settings::iniFilePath();
    }
    else
    {
        QTextStream in(&file);

        ui->textBrowser->setText(in.readAll());

        file.close();
    }
}

void SettingsView::saveSettingsFile()
{
    QFile file(Settings::iniFilePath());
    if(!file.open(QIODevice::WriteOnly))
    {
        //error
        qDebug() << "Unable to open file: " << Settings::iniFilePath();
    }
    else
    {
        file.flush();
        file.write(ui->textBrowser->toPlainText().toUtf8());
        file.close();
    }
}
