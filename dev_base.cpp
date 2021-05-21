#include "dev_base.h"

dev_base::dev_base(QWidget *parent) : QWidget(parent)
{

    MouseEvent = new Mouseenter(this);

    connect(MouseEvent,&Mouseenter::signalMouseHoverEnter,this,[=](QLabel* lab)
    {
        QString str =  lab->toolTip();
        emit DEVMouseEvent(str);
    });

    connect(MouseEvent,&Mouseenter::signalMouseHoverLeave,this,[=]()
    {
        QString str = tr("Наведите курсор чтоб получить информацию...");
        emit DEVMouseEvent(str);
    });

}
dev_base::~dev_base()
{
 delete MouseEvent;
}
