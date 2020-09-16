#ifndef LIBTYPE4_H
#define LIBTYPE4_H
//#include "libType4_global.h"

#include <QtWidgets/QMdiArea>
#include <QDialog>
#include "structs_main.h"


class TestInterface
{
public:
    virtual ~TestInterface()
    {
    }
};

class  Q_DECL_EXPORT SettingsDialog : public TestInterface
{
public:
    explicit SettingsDialog(QMdiArea *mdiArea);
    virtual ~SettingsDialog();
    virtual void testfunc(QMdiArea *mdiArea);
};

#endif // LIBTYPE4_H
