#ifndef QSUDOKUSOLVER_H
#define QSUDOKUSOLVER_H

#include <QWidget>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include <QMessageBox>
#include <QGestureEvent>
#include <QDebug>

#include "solver.h"
#include "sudokupuzzles.h"

namespace Ui {
class QSudokuSolver;
}

class QSudokuSolver : public QWidget
{
    Q_OBJECT

public:
    explicit QSudokuSolver(QWidget *parent = 0);
    ~QSudokuSolver();

    enum SudokuPuzzleIndex
    {
        PUZZLE_CUSTOM = 0U,
    };
    Q_ENUM(SudokuPuzzleIndex)

    enum SudokuMode
    {
        SUDOKUMODE_PLAY = 0U,
        SUDOKUMODE_SOLVE,
    };
    Q_ENUM(SudokuMode)

    void ChangeMode(const QString &ModeString);
    Ui::QSudokuSolver *GetUI(void);
public slots:
    void SolveSucceedProc(void);
    void InvalidSudokuPuzzleProc(void);

    void PuzzlesInitCompleteProc(void);
    void boxPressed(QString boxname);
    void boxReleased(QString boxname);
    void onSpinBoxValueChanged(int value);
    void StepUpTimeOut();
    void StepDownTimeOut();

protected:
    bool event(QEvent *event);

private slots:
    void on_SolveButton_clicked();

    void on_ClearButton_clicked();

    void on_ModeComboBox_currentIndexChanged(const QString &ComboBoxString);

    void on_PuzzleComboBox_currentIndexChanged(int index);

    void on_CheckButton_clicked();

    void on_MakeButton_clicked();

private:
    void CheckButtonEnable(void);
    void CheckButtonDisable(void);
    void MakeButtonEnable(void);
    void MakeButtonDisable(void);
    void SolveButtonEnable(void);
    void SolveButtonDisable(void);
    void ClearButtonDisable(void);
    void ClearButtonEnable(void);

    bool CheckPuzzleInput(void);
    bool PlayModeSolveSucceed(void);
    void SolveModeSolveSucceed(void);
    bool gestureEvent(QGestureEvent *event);
    void pinchTriggered(QPinchGesture *gesture);

private:
    Ui::QSudokuSolver *ui;
    Solver m_Solver;
    bool m_SolvedStatus;
    SudokuPuzzles m_Puzzles;

public:
    bool m_CustomPuzzleMaked;
    int m_SudokuMode;
    QString m_CurrentBoxName;
    QTimer *m_stepup_timer;
    QTimer *m_stepdown_timer;
};

#endif // QSUDOKUSOLVER_H
