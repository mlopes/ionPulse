#include "mainwindow.h"
#include <shared.h>
#include <QStatusBar>

namespace IonHeart {



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(QObject::tr("ionPulse"));
    setStatusBar(new QStatusBar(this));
}

MainWindow::~MainWindow()
{
}


}
