#include "login.h"
#include "ui_login.h"
#include <QPainter>

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

//    QPixmap bkgnd(":/images/bac.png");
//    bkgnd = bkgnd.scaled(size(), Qt::IgnoreAspectRatio);
//    QPalette p = palette(); //copy current, not create new
//    p.setBrush(QPalette::Background, bkgnd);
//    setPalette(p);

//    QBrush *brush; brush = new QBrush;
//    QPalette *palette = new QPalette;
//    brush->setTextureImage(QImage(":/images/bac.png"));
//    palette->setBrush(QPalette::Window, *brush);
//    this->setPalette(*palette);

//    QPixmap pixmapTarget = QPixmap(":/PNG/images/bac1.png");
//    ui->label_3->setPixmap(pixmapTarget);
//    QPixmap pixmapScaled =  pixmapTarget.scaled(5, 5, Qt::KeepAspectRatio, Qt::SmoothTransformation);
// pixmapTarget.scaled(size-5, size-5, Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    QBrush *brush; brush = new QBrush;
//    QPalette *palette = new QPalette;
//    brush->setTextureImage(QImage(":/PNG/images/bac.png"));
//    palette->setBrush(QPalette::Window, *brush);
//    //this->setPalette(*palette);
//    ui->widget->setPalette(*palette);

//    this->setStyleSheet("border-image: url(:/PNG/images/baclogo.png)");
//    ui->button_exit->setStyleSheet("background-color: rgb(170, 0, 0)");

    connect(ui->button_login,&QPushButton::clicked,this,[=]
    {
        QString str = ui->lin_login->text() +":"+ ui->lin_password->text();
        emit clickLogin(str);
    });
    connect(ui->button_exit,&QPushButton::clicked,this,[=]
    {
        emit closeLogin();
    });
}

Login::~Login()
{
    emit closeLogin();
    delete ui;
}
