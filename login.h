#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QDialog>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
signals:
    void clickLogin(QString str);
    void closeLogin();
private:
    Ui::Login *ui;
};

#endif // LOGIN_H
