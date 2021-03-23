#ifndef SUDOKUPUZZLES_H
#define SUDOKUPUZZLES_H

#include <QObject>
#include <QList>
#include <QByteArray>
#include <QTimer>

class SudokuPuzzles : public QObject
{
    Q_OBJECT
public:
    explicit SudokuPuzzles(QObject *parent = nullptr);

signals:
    void initComplete(void);

public slots:

private:
    void initPuzzleList(void);

public:
    QList<QByteArray> m_SudokuPuzzleList;
};

#endif // SUDOKUPUZZLES_H
