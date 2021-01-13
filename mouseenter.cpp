#include "mouseenter.h"

Mouseenter::Mouseenter(QObject *parent) : QObject(parent)
{

}

void Mouseenter::attach(QLabel *label)
{
    label->setAttribute(Qt::WA_Hover, true);
    label->installEventFilter(this);
}

bool Mouseenter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter)
    {
        emit signalMouseHoverEnter(dynamic_cast<QLabel*>(obj) );
    }
    if (event->type() == QEvent::HoverLeave)
    {
        emit signalMouseHoverLeave(dynamic_cast<QLabel*>(obj) );
    }
    return false;
}
