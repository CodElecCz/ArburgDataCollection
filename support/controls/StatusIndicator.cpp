#include "StatusIndicator.h"

#include <QPainter>
#include <QFontMetrics>

namespace
{    
    const int cStatusIndicatorOffset = 3;
    const int cStatusIndicatorBlinkMs = 250;
}

StatusIndicator::StatusIndicator(const QString &text, const QColor &color, QWidget *parent) :
    QWidget(parent),
    m_color(color),
    m_on(false),
    m_text(text)
{
    QFontMetrics fm(font());
    int fmWidth = fm.boundingRect(text).width();
    int fmHeight = fm.boundingRect(text).height();
    this->setMinimumSize(fmWidth + fmHeight, fmHeight);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateTime()));
}

StatusIndicator::~StatusIndicator()
{
    m_timer.stop();
}

bool StatusIndicator::isOn() const
{
    return m_on;
}

void StatusIndicator::setOn(bool on)
{
    if (on == m_on)
        return;

    m_on = on;
    update();
}

void StatusIndicator::setOn(bool on, const QColor &color)
{
    if (on == m_on)
        return;

    m_on = on;
    m_color = color;
    update();
}

QColor StatusIndicator::color() const
{
    return m_color;
}

void StatusIndicator::setColor(QColor color)
{
    if (color == m_color)
        return;

    m_color = color;
    update();
}

QString StatusIndicator::text() const
{
    return m_text;
}

void StatusIndicator::setText(const QString &text)
{
    if (0==text.compare(m_text))
        return;

    m_text = text;
    update();
}

QString StatusIndicator::msg() const
{
    return m_msg;
}

void StatusIndicator::setMsg(const QString& msg)
{
    if (msg.compare(m_msg)==0)
        return;

    m_msg = msg;
    update();
}

void StatusIndicator::setMsg(const QString& msg, const QColor &color)
{
    if (msg.compare(m_msg)==0)
        return;

    m_msg = msg;
    m_color = color;
    update();
}

void StatusIndicator::blink(int time)
{
    m_time = time;
    m_timer.setInterval(cStatusIndicatorBlinkMs);
    m_timer.start();
}

void StatusIndicator::turnOff()
{
    setOn(false);
}

void StatusIndicator::turnOn()
{
    setOn(true);
}

void StatusIndicator::updateTime()
{
    m_time -= cStatusIndicatorBlinkMs;
    if(m_time<=0)
    {
        m_timer.stop();
        setOn(true);
    }
    else
    {
        setOn(!isOn());
    }
}

void StatusIndicator::paintEvent(QPaintEvent *)
{
    paint(m_on? m_color : palette().color(QPalette::AlternateBase));
}

void StatusIndicator::paint(QColor color)
{
    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(color);

    QFontMetrics fm(font());
    int fmHeight = fm.boundingRect(m_text).height();

    p.drawEllipse(cStatusIndicatorOffset, cStatusIndicatorOffset, height()-2*cStatusIndicatorOffset, height()-2*cStatusIndicatorOffset);

    int textOffsetY = (height() - fmHeight)/2;
    if(m_msg.length()==0)
    {
        int fmWidth = fm.boundingRect(m_text).width();
        p.drawText(height(), textOffsetY, fmWidth, fmHeight, Qt::AlignVCenter, m_text);
    }
    else
    {
        QString text = m_text + " [" + m_msg + "]";        
        int fmWidth = fm.boundingRect(text).width();

        p.drawText(height(), textOffsetY, fmWidth, fmHeight, Qt::AlignVCenter, text);

        setMinimumWidth(fmWidth + height());
    }
}
