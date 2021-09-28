#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QDialog>
#include <QTimer>

#include "support/controls/WaitingSpinner.h"

class WaitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitDialog(QWidget *parent = nullptr);
    ~WaitDialog();

    void show(int timeoutMs = -1);
    void close(int delayMs = -1);

    static WaitDialog& instance();

private slots:
    void delayedShow();
    void delayedClose();

private:
    WaitingSpinner m_waitingSpinner;
    QTimer         m_timeout;
    QTimer         m_delayedShow;
    QTimer         m_delayedClose;
};

#endif // WAITDIALOG_H
