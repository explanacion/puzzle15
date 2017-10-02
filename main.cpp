#include "puzzle15.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Puzzle15 w;
    //w.setGeometry(0,0,w.width(),w.height());
    w.setMinimumSize(200,200);
    w.setGeometry(0,0,QApplication::desktop()->width(),QApplication::desktop()->height());
    qDebug() << QApplication::desktop()->height();
    w.setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint); // remove the titlebar
    w.setWindowState(Qt::WindowActive & Qt::WindowFullScreen);
    w.showNormal();
    return a.exec();
}
