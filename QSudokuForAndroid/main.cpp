#include "qsudokusolver.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QSudokuSolver w;

    qDebug() << "Qt::GestureFlags()" << Qt::GestureFlags();
    w.grabGesture(Qt::PanGesture);
    w.grabGesture(Qt::PinchGesture);
    w.grabGesture(Qt::SwipeGesture);

    w.show();

    return a.exec();
}
