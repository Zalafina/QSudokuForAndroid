#include "stdio.h"
#include "string.h"

#include <QDebug>
#include "solver.h"

QHash<quint32, QString> Solver::m_BoxMap = QHash<quint32, QString>();
bool Solver::invalid_puzzle = false;

static const int SOLVE_COUNT_MAX = 5000000;
static const int BLANK = 0;
static const int ONES = 0x3fe;   // Binary 1111111110

Solver::Solver(QObject *parent) : QObject(parent)
{
    SolverInit();

    initBoxMap();
}

quint32 Solver::index2row(quint32 index)
{
    return (index-1)/9 + 1;
}

quint32 Solver::index2col(quint32 index)
{
    return index - (index-1)/9*9;
}

void Solver::SudokuSolve(const char (&matrix)[9][9])
{
    CollectInput(matrix);
    Place(SeqPtr);

    if ((1 == SolvedCounter)
            && (1 == m_EntryResult.size())){
        emit SolveSucceed();
    }
    else{
#ifdef DEBUG_LOGOUT_ON
        putc('\n', stdout);
        printf("Invalid Sudoku Puzzle Input!\n");
        fflush(stdout);
#endif
        SolverInit();
        emit InvalidSudokuPuzzle();
    }
}

void Solver::GetEntryResult(int *entrybuffer)
{
    if (entrybuffer != nullptr){
        if (1 == m_EntryResult.size()){
            ENTRY_RESULT result = m_EntryResult.constFirst();
            memcpy(entrybuffer, result.EntryResult, sizeof(result.EntryResult));
        }
    }
}

void Solver::ClearSolvedStatus()
{
    SolverInit();
}

void Solver::SolverInit(void)
{
    int i, j, Square;

    for (i = 0; i < 9; i++){
        for (j = 0; j < 9; j++) {
            Square = 9 * i + j;
            InRow[Square] = i;
            InCol[Square] = j;
            InBlock[Square] = (i / 3) * 3 + ( j / 3);
        }
    }


    for (Square = 0; Square < 81; Square++) {
        Sequence[Square] = Square;
        Entry[Square] = BLANK;
        LevelCount[Square] = 0;
    }

    for (i = 0; i < 9; i++){
        Block[i] = Row[i] = Col[i] = ONES;
    }

    SeqPtr = 0;
    Count = 0;
    SolvedCounter = 0;
    m_EntryResult.clear();
    invalid_puzzle = false;
}

void Solver::initBoxMap(void)
{
    for(int loop = 1; loop <= 81; loop++){
        int row = (loop-1)/9 + 1;
        int col = loop - (loop-1)/9*9;
        QString str_row, str_col, box_name;
        str_row.setNum(row);
        str_row.prepend(QChar('R'));
        str_col.setNum(col);
        str_col.prepend(QChar('C'));
        box_name = QString("Box") + str_row + str_col;
        m_BoxMap.insert(loop, box_name);

#ifdef DEBUG_LOGOUT_ON
        //qDebug("key(%d), row(%d), col(%d), str_row(%s), str_col(%s), box_name(%s)", loop, row, col, str_row.toStdString().c_str(), str_col.toStdString().c_str(), box_name.toStdString().c_str());
#endif
    }
}

void Solver::CollectInput(const char (&matrix)[9][9])
{
    int row, col;
    char InputString[10];

#ifdef DEBUG_LOGOUT_ON
    printf("Sudoku Puzzle Input:");
#endif
    for (row = 0; row < 9; row++) {
        memset(InputString, 0x00, sizeof(InputString));
        memcpy(InputString, matrix[row], 9);

#ifdef DEBUG_LOGOUT_ON
        //qDebug("Row[%d] : %s", row + 1, InputString);
#endif

        for (col = 0; col < 9; col++) {
            char ch = InputString[col];
            if (ch >= '1' && ch <= '9'){
                InitEntry(row, col, ch - '0');
            }
        }
    }
#ifdef DEBUG_LOGOUT_ON
    PrintArray();
    putc('\n', stdout);
    fflush(stdout);
#endif
}

void Solver::InitEntry(int row, int col, int val)
{
    int Square = 9 * row + col;
    int valbit = 1 << val;
    int SeqPtr2;

    // add suitable checks for data consistency

    Entry[Square] = valbit;
    Block[InBlock[Square]] &= ~valbit;
    Col[InCol[Square]] &= ~valbit; // Simpler Col[j] &= ~valbit;
    Row[InRow[Square]] &= ~valbit; // Simpler Row[i] &= ~valbit;

    SeqPtr2 = SeqPtr;
    while (SeqPtr2 < 81 && Sequence[SeqPtr2] != Square)
        SeqPtr2++ ;

    SwapSeqEntries(SeqPtr, SeqPtr2);
    SeqPtr++;
}

void Solver::PrintArray(void)
{
#ifdef DEBUG_LOGOUT_ON
    int row, col, valbit, val, Square;
    char ch;

    Square = 0;

    for (row = 0; row < 9; row++) {
        if (row % 3 == 0){
            putc('\n', stdout);
        }
        for (col = 0; col < 9; col++) {
            if (col % 3 == 0){
                putc(' ', stdout);
            }
            valbit = Entry[Square++];
            if (valbit == 0){
                ch = '-';
            }
            else {
                for (val = 1; val <= 9; val++)
                    if (valbit == (1 << val)) {
                        ch = '0' + val;
                        break;
                    }
            }
            putc(ch,stdout);
        }
        putc ('\n', stdout);
    }

    fflush(stdout);
#endif
}

void Solver::SwapSeqEntries(int S1, int S2)
{
    int temp = Sequence[S2];
    Sequence[S2] = Sequence[S1];
    Sequence[S1] = temp;
}

int Solver::NextSeq(int S)
{
    int S2, Square, Possibles, BitCount;
    int T, MinBitCount = 100;

    for (T = S; T < 81; T++) {
        Square = Sequence[T];
        Possibles = Block[InBlock[Square]] & Row[InRow[Square]] & Col[InCol[Square]];
        BitCount = 0;
        while (Possibles) {
            Possibles &= ~(Possibles & -Possibles);
            BitCount++;
        }

        if (BitCount < MinBitCount) {
            MinBitCount = BitCount;
            S2 = T;
        }
    }

    return S2;
}

void Solver::Place(int S)
{
    Count++;

    if (Count > SOLVE_COUNT_MAX){
        invalid_puzzle = true;
    }

    if (S >= 81) {
        SolvedCounter++;

        if (1 == SolvedCounter){
            Succeed();
        }
        else{
            invalid_puzzle = true;
        }
        return;
    }

    LevelCount[S]++;

    int S2 = NextSeq(S);
    SwapSeqEntries(S, S2);

    int Square = Sequence[S];

    int   BlockIndex = InBlock[Square],
            RowIndex = InRow[Square],
            ColIndex = InCol[Square];

    int   Possibles = Block[BlockIndex] & Row[RowIndex] & Col[ColIndex];
    while (Possibles && (false == invalid_puzzle)) {
        int valbit = Possibles & (-Possibles); // Lowest 1 bit in Possibles
        Possibles &= ~valbit;
        Entry[Square] = valbit;
        Block[BlockIndex] &= ~valbit;
        Row[RowIndex] &= ~valbit;
        Col[ColIndex] &= ~valbit;

        Place(S + 1);

        Entry[Square] = BLANK; // Could be moved out of the loop
        Block[BlockIndex] |= valbit;
        Row[RowIndex] |= valbit;
        Col[ColIndex] |= valbit;
    }

    SwapSeqEntries(S, S2);
}

void Solver::PrintStats(void)
{
#ifdef DEBUG_LOGOUT_ON
    int i, S;
    printf("\nLevel Counts:\n\n");

    S = 0;
    while (LevelCount[S] == 0) S++;

    i = 0;

    while (S < 81) {
        int Seq = Sequence[S];
        printf("(%d, %d):%4d ", Seq / 9 + 1, Seq % 9 + 1, LevelCount[S]);
        if (i++ > 4){
            printf("\n");
            i = 0;
        }
        S++;
    }

    printf("\n\nCount = %d\n", Count);
#endif
}

void Solver::Succeed(void)
{
    ENTRY_RESULT solve_result;
    memcpy(solve_result.EntryResult, Entry, sizeof(Entry));
    m_EntryResult.append(solve_result);

#ifdef DEBUG_LOGOUT_ON
    printf("Sudoku Puzzle Solve Complete:Count(%d)", Count);
    PrintArray();
    //PrintStats();
#endif
}
