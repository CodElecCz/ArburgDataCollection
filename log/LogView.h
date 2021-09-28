#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QWidget>

namespace Ui {
class LogView;
}

class LogView : public QWidget
{
    Q_OBJECT

public:
    explicit LogView(QWidget *parent = nullptr);
    ~LogView();

public slots:
    void message(const QString& msg);

private:
    Ui::LogView *ui;
};

#endif // LOGVIEW_H
