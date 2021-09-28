#ifndef STATUSINDICATOR_H
#define STATUSINDICATOR_H

#include <QWidget>
#include <QObject>
#include <QTimer>
#include <QColor>

class StatusIndicator : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString msg READ msg WRITE setMsg)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    StatusIndicator(const QString& text, const QColor &color, QWidget *parent = nullptr);
    ~StatusIndicator();

    bool isOn() const;
    void setOn(bool on);
    void setOn(bool on, const QColor &color);

    QColor color() const;
    void setColor(QColor color);

    QString text() const;
    void setText(const QString& text);

    QString msg() const;
    void setMsg(const QString& msg);
    void setMsg(const QString& msg, const QColor &color);

    void blink(int time = 1500);

public slots:
    void turnOff();
    void turnOn();

private slots:
    void updateTime();

protected:
    void paintEvent(QPaintEvent *) override;
    void paint(QColor color);

private:
    QColor  m_color;
    bool    m_on;
    int     m_time;
    QTimer  m_timer;
    QString m_text;
    QString m_msg;
};

#endif // STATUSINDICATOR_H
