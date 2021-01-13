#ifndef MOUSEENTER_H
#define MOUSEENTER_H

#include <QObject>
#include <QLabel>
#include <QEvent>

class Mouseenter : public QObject
{
    Q_OBJECT
public:
    explicit Mouseenter(QObject *parent = nullptr);
    virtual ~Mouseenter() {};

    void attach(QLabel *label);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void signalMouseHoverEnter(QLabel*);
    void signalMouseHoverLeave(QLabel*);
};

#endif // MOUSEENTER_H
