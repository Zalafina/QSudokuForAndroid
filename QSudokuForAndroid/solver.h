#ifndef SOLVER_H
#define SOLVER_H

#include <QObject>
#include <QHash>

typedef struct
{
    int EntryResult[81];
}ENTRY_RESULT;

class Solver : public QObject
{
    Q_OBJECT
public:
    explicit Solver(QObject *parent = 0);

signals:
    void SolveSucceed(void);
    void InvalidSudokuPuzzle(void);

public slots:

public:
    static quint32 index2row(quint32 index);
    static quint32 index2col(quint32 index);

    void SudokuSolve(const char (&matrix)[9][9]);
    void GetEntryResult(int *entrybuffer);
    void ClearSolvedStatus(void);

private:
    void SolverInit(void);
    void initBoxMap(void);

    void CollectInput(const char (&matrix)[9][9]);
    void InitEntry(int row, int col, int val);
    void PrintArray(void);
    void SwapSeqEntries(int S1, int S2);
    int NextSeq(int S);
    void Place(int S);
    void PrintStats(void);
    void Succeed(void);

public:
    static QHash<quint32, QString> m_BoxMap;

private:
    static bool invalid_puzzle;

    int InBlock[81], InRow[81], InCol[81];

    int Entry[81];  // Records entries 1-9 in the grid, as the corresponding bit set to 1
    int Block[9], Row[9], Col[9]; // Each int is a 9-bit array

    int SeqPtr = 0;
    int Sequence[81];

    int Count = 0;
    int LevelCount[81];

    int SolvedCounter;
    QList<ENTRY_RESULT> m_EntryResult;
};

#endif // SOLVER_H
