#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSharedMemory shared_memory;
    shared_memory.setKey(QString(argv[0]));
    if(shared_memory.attach())
    {
        return 0;
    }
    if(shared_memory.create(1))
    {
        //Widget::AutoRunWithSystem(true,"Launch");
        Widget w;
        w.show();
        return a.exec();
    }
}
