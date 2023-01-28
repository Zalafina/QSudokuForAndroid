#include "qsudokusolver.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Unset);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QSudokuSolver w;
    w.grabGesture(Qt::PinchGesture);
    w.show();

    return a.exec();
}
