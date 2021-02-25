#include "login.h"
#include "ui_login.h"
#include <QPainter>
#include <QFile>

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
        QString str_login = ui->lin_login->text();
        QString str_pas = ui->lin_password->text();
        ui->lin_password->clear();

        static const char* const FILE_NAME = "login.bin";
        if( ui->checkBox->isChecked())
        {
            QFile file( FILE_NAME );
            QDataStream stream( &file );
            file.open( QIODevice::WriteOnly );
            if (file.isOpen())
            {
                stream << str_login;
                stream << str_pas;
            }
        }else
        {
           QFile file( FILE_NAME );
           file.remove();
        }
        if(checkPass(str_login,str_pas))
        emit clickLogin(str_login);
        else
        {
            ui->lin_login->clear();
            ui->lin_password->clear();
        }

    });
    connect(ui->button_exit,&QPushButton::clicked,this,[=]
    {
        static const char* const FILE_NAME = "login.bin";
        QFile file( FILE_NAME );
        file.remove();
        emit closeLogin();
    });
}

Login::~Login()
{
    delete ui;
}

void Login::checkLogin()
{
    QString str_login;
    QString str_pas;

    static const char* const FILE_NAME = "login.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );

    file.open( QIODevice::ReadOnly );
    if (file.isOpen())
    {
        stream >> str_login;
        stream >> str_pas;
    }
    if(checkPass(str_login,str_pas))
    {
        emit clickLogin(str_login);
    }
}

bool Login::checkPass(QString login,QString pass)
{
 bool stat =false;
 if(login.length()>0)
     if(pass =="100")
         stat =true;
 return stat;
}

void Login::retranslate()
{
    ui->retranslateUi(this);
}
