#include <QDebug>
#include <string.h>
#include "qsudokusolver.h"
#include "ui_qsudokusolver.h"

QSudokuSolver::QSudokuSolver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QSudokuSolver),
    m_Solver(this),
    m_SolvedStatus(false),
    m_Puzzles(this),
    m_CustomPuzzleMaked(false),
    m_SudokuMode(SUDOKUMODE_PLAY),
    m_CurrentBoxName()
{
    ui->setupUi(this);

    CheckButtonDisable();
    MakeButtonEnable();
    SolveButtonEnable();
    ui->ClearButton->setStyleSheet("color: red");
    ui->SetValueSpinBox->setVisible(false);
    ui->SetValueSpinBox->setStyleSheet("background-color: mistyrose");
    ui->SetValueSpinBox->setReadOnly(true);

    ChangeMode(QString("Play Sudoku"));

    QObject::connect(&m_Solver, &Solver::SolveSucceed, this, &QSudokuSolver::SolveSucceedProc);
    QObject::connect(&m_Solver, &Solver::InvalidSudokuPuzzle, this, &QSudokuSolver::InvalidSudokuPuzzleProc);
    QObject::connect(&m_Puzzles, &SudokuPuzzles::initComplete, this, &QSudokuSolver::PuzzlesInitCompleteProc);
}

QSudokuSolver::~QSudokuSolver()
{
    delete ui;
}

void QSudokuSolver::ChangeMode(const QString &ModeString)
{
    if (ModeString == QString("Play Sudoku")){
        m_SudokuMode = SUDOKUMODE_PLAY;
    }
    else if (ModeString == QString("Solve Sudoku")){
        m_SudokuMode = SUDOKUMODE_SOLVE;
    }

    if (PUZZLE_CUSTOM == ui->PuzzleComboBox->currentIndex()){
        on_PuzzleComboBox_currentIndexChanged(PUZZLE_CUSTOM);
    }
    else{
        ui->PuzzleComboBox->setCurrentIndex(PUZZLE_CUSTOM);
    }
}

Ui::QSudokuSolver *QSudokuSolver::GetUI(void)
{
    return ui;
}

void QSudokuSolver::SolveSucceedProc(void)
{
    if (SUDOKUMODE_SOLVE == m_SudokuMode){
        SolveModeSolveSucceed();
    }
    else{
        if ((SUDOKUMODE_PLAY == m_SudokuMode)
                && (PUZZLE_CUSTOM == ui->PuzzleComboBox->currentIndex())
                && (false == m_CustomPuzzleMaked)){
            m_CustomPuzzleMaked = true;
            MakeButtonDisable();
            ClearButtonEnable();
            CheckButtonEnable();
        }
        else{
            bool checkresult = PlayModeSolveSucceed();
            if (true == checkresult){
    #ifdef DEBUG_LOGOUT_ON
                qDebug() << "Puzzle"<<ui->PuzzleComboBox->currentIndex()<<"Complete";
    #endif
                QMessageBox::information(this, tr("QKeyMapper"), tr("<html><head/><body><p align=\"center\"><font color='green'>Sudoku Puzzle Complete.</font></p></body></html>"));
            }
            else{
                QMessageBox::warning(this, tr("QKeyMapper"), tr("<html><head/><body><font color='red'><p align=\"center\">Incorrect sudoku solution.</p><p align=\"center\">Try again please!</p></font></body></html>"));
            }
        }
    }
}

void QSudokuSolver::InvalidSudokuPuzzleProc(void)
{
    m_SolvedStatus = false;

    if (SUDOKUMODE_SOLVE == m_SudokuMode){
        QMessageBox::warning(this, tr("QKeyMapper"), tr("<html><head/><body><font color='red'><p align=\"center\">Input invalid Sudoku Puzzle.</p><p align=\"center\">Reinput it please!</p></font></body></html>"));
    }
    else{
        if (PUZZLE_CUSTOM == ui->PuzzleComboBox->currentIndex()){
            QMessageBox::warning(this, tr("QKeyMapper"), tr("<html><head/><body><font color='red'><p align=\"center\">Input invalid Sudoku Puzzle.</p><p align=\"center\">Reinput it please!</p></font></body></html>"));
        }
        else{
            qDebug() << "PlayCustomMode invalid puzzle index:" << ui->PuzzleComboBox->currentIndex();
        }
    }
}

void QSudokuSolver::PuzzlesInitCompleteProc(void)
{
    int puzzletablesize = m_Puzzles.m_SudokuPuzzleList.size();

    for (int loop = 1; loop <= puzzletablesize; loop++){
        QString puzzle_number;
        puzzle_number = QString("%1").arg(loop, 3, 10, QChar('0'));
        QString combobox_String = QString("Puzzle ") + puzzle_number;
        ui->PuzzleComboBox->addItem(combobox_String);
    }
}

void QSudokuSolver::boxPressed(QString boxname)
{
    qDebug() << "boxPressed:" << boxname;
    m_CurrentBoxName = boxname;
}

void QSudokuSolver::boxReleased(QString boxname)
{
    qDebug() << "boxReleased:" << boxname;
    m_CurrentBoxName.clear();
}

bool QSudokuSolver::event(QEvent *event)
{
    qDebug() << "EventType:" << event->type();
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}

void QSudokuSolver::on_SolveButton_clicked()
{
    char matrix[9][9];
    memset(matrix, '-', sizeof(matrix));
    for (const QString &boxname : m_Solver.m_BoxMap)
    {
        QSudouBox *box = this->findChild<QSudouBox *>(boxname);
        if (box != nullptr){
            quint32 index = m_Solver.m_BoxMap.key(boxname);
            int row = Solver::index2row(index);
            int col = Solver::index2col(index);

            QString boxtext = box->text();
            bool isNumber = false;
            int value = boxtext.toInt(&isNumber);
            if ((false == boxtext.isEmpty())
                    && (QSudouBox::BOXTYPE_PUZZLE == box->m_BoxType)
                    && (true == isNumber)
                    && (1<= value && value <= 9)){
                matrix[row-1][col-1] = boxtext.toStdString().c_str()[0];
            }

#ifdef DEBUG_LOGOUT_ON
            //qDebug("index(%d), row(%d), col(%d), name(%s)", index, Solver::index2row(index), Solver::index2col(index), boxname.toStdString().c_str());
#endif
        }
        else{
            qDebug() << "SolveButton Find BoxName Error:" << boxname;
        }
    }

    m_Solver.SudokuSolve(matrix);
}

void QSudokuSolver::on_ClearButton_clicked()
{
    if (SUDOKUMODE_PLAY == m_SudokuMode){
        for (const QString &boxname : m_Solver.m_BoxMap)
        {
            QSudouBox *box = this->findChild<QSudouBox *>(boxname);
            if (box != nullptr){
                if (QSudouBox::BOXTYPE_FILL == box->m_BoxType){
                    box->setEnabled(true);
                    //box->setFocusPolicy(Qt::ClickFocus);
                    box->clearall();
                }
            }
            else{
                qDebug() << "on_ClearButton_clicked Find BoxName Error:" << boxname;
            }
        }

        m_SolvedStatus = false;
        m_Solver.ClearSolvedStatus();
    }
    else{
        if (PUZZLE_CUSTOM == ui->PuzzleComboBox->currentIndex()){
            on_PuzzleComboBox_currentIndexChanged(PUZZLE_CUSTOM);
        }
        else{
            ui->PuzzleComboBox->setCurrentIndex(PUZZLE_CUSTOM);
        }
    }
}

void QSudokuSolver::on_ModeComboBox_currentIndexChanged(const QString &ComboBoxString)
{
    if (ComboBoxString == QString("Play Sudoku")){
#ifdef DEBUG_LOGOUT_ON
        qDebug() << "Mode Changed:" << ComboBoxString;
#endif

        ChangeMode(ComboBoxString);
    }
    else if (ComboBoxString == QString("Solve Sudoku")){
#ifdef DEBUG_LOGOUT_ON
        qDebug() << "Mode Changed:" << ComboBoxString;
#endif

        ChangeMode(ComboBoxString);
    }
}

void QSudokuSolver::on_PuzzleComboBox_currentIndexChanged(int index)
{
#ifdef DEBUG_LOGOUT_ON
    qDebug() << "PuzzleComboBox IndexChanged:" << index;
#endif

    if (PUZZLE_CUSTOM == index){
        for (const QString &boxname : m_Solver.m_BoxMap)
        {
            QSudouBox *box = this->findChild<QSudouBox *>(boxname);
            if (box != nullptr){
                box->setEnabled(true);
                //box->setFocusPolicy(Qt::ClickFocus);
                box->clearall();
            }
            else{
                qDebug() << "on_PuzzleComboBox_currentIndexChanged(0) Find BoxName Error:" << boxname;
            }
        }

        m_SolvedStatus = false;
        m_Solver.ClearSolvedStatus();

        if (SUDOKUMODE_PLAY ==  m_SudokuMode){
            ui->SolveButton->setVisible(false);
            ui->ClearButton->setVisible(false);

            CheckButtonDisable();
            MakeButtonEnable();
            ui->CheckButton->setVisible(true);
            ui->MakeButton->setVisible(true);
        }
        else{
            ui->CheckButton->setVisible(false);
            ui->MakeButton->setVisible(false);

            MakeButtonDisable();
            SolveButtonEnable();
            ui->SolveButton->setVisible(true);
            ui->ClearButton->setVisible(true);

        }
    }
    else if (index <= m_Puzzles.m_SudokuPuzzleList.size()){
#ifdef DEBUG_LOGOUT_ON
        qDebug() << "Sudoku:" << m_Puzzles.m_SudokuPuzzleList.at(index-1);
#endif
        for (const QString &boxname : m_Solver.m_BoxMap)
        {
            QSudouBox *box = this->findChild<QSudouBox *>(boxname);
            if (box != nullptr){
                box->setEnabled(true);
                //box->setFocusPolicy(Qt::ClickFocus);
                box->clearall();
            }
            else{
                qDebug() << "on_PuzzleComboBox_currentIndexChanged(0) Find BoxName Error:" << boxname;
            }
        }

        m_SolvedStatus = false;
        m_Solver.ClearSolvedStatus();
        CheckButtonEnable();
        SolveButtonEnable();
        MakeButtonDisable();
        ClearButtonEnable();

        QByteArray sudoku_puzzle = m_Puzzles.m_SudokuPuzzleList.at(index-1);

        for(int index = 1; index <= sudoku_puzzle.size(); index++){
            int row = Solver::index2row(index);
            int col = Solver::index2col(index);
            QString str_row, str_col, box_name;
            str_row.setNum(row);
            str_row.prepend(QChar('R'));
            str_col.setNum(col);
            str_col.prepend(QChar('C'));
            box_name = QString("Box") + str_row + str_col;

            //set puzzle number to Box Text
            QSudouBox *box = this->findChild<QSudouBox *>(box_name);
            if (box != nullptr){
                char number = sudoku_puzzle.at(index-1);
                if (number != '.'){
                    box->setText(QString(number));
                    //box->setFocusPolicy(Qt::NoFocus);
                    box->setpuzzlenumber();
                    box->setEnabled(false);
                }
            }
            else{
                qDebug("on_PuzzleComboBox_currentIndexChanged(%d) Find BoxName Error:%s", index, box_name.toStdString().c_str());
            }

        }
    }
    else{
        qDebug() << "PuzzleComboBox IndexError:" << index;
    }
}

void QSudokuSolver::on_CheckButton_clicked()
{
    if(SUDOKUMODE_PLAY == m_SudokuMode){
        if (ui->PuzzleComboBox->currentIndex() != PUZZLE_CUSTOM){
            bool result = CheckPuzzleInput();
            if (false == result){
                QMessageBox::warning(this, tr("QKeyMapper"), tr("<html><head/><body><font color='red'><p align=\"center\">Incomplete Sudoku Puzzle.</p><p align=\"center\">Continue to complete it please!</p></font></body></html>"));
            }
        }
        else{
            if (true == m_CustomPuzzleMaked){
                bool result = CheckPuzzleInput();
                if (false == result){
                    QMessageBox::warning(this, tr("QKeyMapper"), tr("<html><head/><body><font color='red'><p align=\"center\">Incomplete Sudoku Puzzle.</p><p align=\"center\">Continue to complete it please!</p></font></body></html>"));
                }
            }
            else{
                qDebug() << "CheckButton click CustomPuzzleMaked Status Error!!!";
            }
        }
    }
    else{
        qDebug() << "CheckButton click Mode Error!!!";
    }
}

void QSudokuSolver::CheckButtonEnable(void)
{
    ui->CheckButton->setEnabled(true);
    ui->CheckButton->setStyleSheet("color: forestgreen");
}

void QSudokuSolver::CheckButtonDisable(void)
{
    ui->CheckButton->setEnabled(false);
    ui->CheckButton->setStyleSheet("color: darkgray");
}

void QSudokuSolver::MakeButtonEnable(void)
{
    ui->MakeButton->setEnabled(true);
    ui->MakeButton->setStyleSheet("color: orchid");
    ui->MakeButton->setVisible(true);
    ui->ClearButton->setVisible(false);
}

void QSudokuSolver::MakeButtonDisable(void)
{
    ui->MakeButton->setEnabled(false);
    ui->MakeButton->setStyleSheet("color: darkgray");
    ui->MakeButton->setVisible(false);
}

void QSudokuSolver::SolveButtonEnable(void)
{
    ui->SolveButton->setEnabled(true);
    ui->SolveButton->setStyleSheet("color: royalblue");
    ui->SolveButton->setText("Solve");
}

void QSudokuSolver::SolveButtonDisable(void)
{
    ui->SolveButton->setEnabled(false);
    ui->SolveButton->setStyleSheet("color: darkgray");
    ui->SolveButton->setText("Solved");
}

void QSudokuSolver::ClearButtonDisable()
{
    ui->ClearButton->setVisible(false);
}

void QSudokuSolver::ClearButtonEnable()
{
    ui->ClearButton->setVisible(true);
}

bool QSudokuSolver::CheckPuzzleInput(void)
{
    bool result = true;
    char matrix[9][9];
    memset(matrix, '-', sizeof(matrix));
    int index = ui->PuzzleComboBox->currentIndex();

    for (const QString &boxname : m_Solver.m_BoxMap)
    {
        QSudouBox *box = this->findChild<QSudouBox *>(boxname);
        if (box != nullptr){
            if (true == box->text().isEmpty()){
                result = false;
                break;
            }
            else{
                if ((PUZZLE_CUSTOM == index)
                        && (QSudouBox::BOXTYPE_PUZZLE == box->m_BoxType)){
                    int box_index = Solver::m_BoxMap.key(box->objectName());
                    int row = Solver::index2row(box_index);
                    int col = Solver::index2col(box_index);
                    matrix[row-1][col-1] = box->text().toStdString().c_str()[0];
                }
            }
        }
        else{
            result = false;
            qDebug() << "CheckPuzzleInput Find BoxName Error:" << boxname;
            break;
        }
    }

    if (true == result){
        if (PUZZLE_CUSTOM == index){
            m_Solver.SudokuSolve(matrix);
        }
        else{
            QByteArray sudoku_puzzle = m_Puzzles.m_SudokuPuzzleList.at(index-1);
            memcpy(matrix, sudoku_puzzle.constData(), sudoku_puzzle.size());

            m_Solver.SudokuSolve(matrix);
        }
    }

    return result;
}

bool QSudokuSolver::PlayModeSolveSucceed(void)
{
    bool checkresult = true;
    int row, col, valbit, val, Square = 0;
    char ch;
    int EntryBuffer[81];
    m_Solver.GetEntryResult(EntryBuffer);
    m_SolvedStatus = false;
    m_Solver.ClearSolvedStatus();

    for (row = 0; (row < 9 && true == checkresult); row++) {
        for (col = 0; (col < 9 && true == checkresult); col++) {
            QString str_row, str_col, box_name;
            str_row.setNum(row+1);
            str_row.prepend(QChar('R'));
            str_col.setNum(col+1);
            str_col.prepend(QChar('C'));
            box_name = QString("Box") + str_row + str_col;

            valbit = EntryBuffer[Square++];
            if (valbit != 0){
                for (val = 1; val <= 9; val++){
                    if (valbit == (1 << val)) {
                        ch = '0' + val;
                        break;
                    }
                }

                //set ch to Box Text
                QSudouBox *box = this->findChild<QSudouBox *>(box_name);
                if (box != nullptr){
                    if(false == box->text().isEmpty()){
                        if (QString(ch) != box->text()){
                            checkresult = false;
                        }
                    }
                }
                else{
                    checkresult = false;
                    qDebug() << "SolveSucceedProc Find BoxName Error:" << box_name;
                }
            }
            else {
            }
        }
    }

    return checkresult;
}

void QSudokuSolver::SolveModeSolveSucceed(void)
{
    int row, col, valbit, val, Square = 0;
    char ch;

    int EntryBuffer[81];
    m_Solver.GetEntryResult(EntryBuffer);

    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            QString str_row, str_col, box_name;
            str_row.setNum(row+1);
            str_row.prepend(QChar('R'));
            str_col.setNum(col+1);
            str_col.prepend(QChar('C'));
            box_name = QString("Box") + str_row + str_col;

            valbit = EntryBuffer[Square++];
            if (valbit != 0){
                for (val = 1; val <= 9; val++){
                    if (valbit == (1 << val)) {
                        ch = '0' + val;
                        break;
                    }
                }

                //set ch to Box Text
                QSudouBox *box = this->findChild<QSudouBox *>(box_name);
                if (box != nullptr){
                    if((QSudouBox::BOXTYPE_BLANK == box->m_BoxType)
                            && (true == box->text().isEmpty())){
                        box->setText(QString(ch));
                    }
                }
                else{
                    qDebug() << "SolveSucceedProc Find BoxName Error:" << box_name;
                }
            }
            else {
            }
        }
    }

    SolveButtonDisable();

    for (const QString &boxname : m_Solver.m_BoxMap)
    {
        QSudouBox *box = this->findChild<QSudouBox *>(boxname);
        if (box != nullptr){
            box->setEnabled(false);
        }
        else{
            qDebug() << "ClearButton Find BoxName Error:" << boxname;
        }
    }

    m_SolvedStatus = true;
}

bool QSudokuSolver::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture)){
        qDebug() << "Qt::PinchGesture";
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    }

    return true;
}

void QSudokuSolver::pinchTriggered(QPinchGesture *gesture)
{
    if (true == ui->SetValueSpinBox->isVisible()){
        QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
        if (changeFlags & QPinchGesture::ScaleFactorChanged)
        {
            qreal value = gesture->property("scaleFactor").toReal();
            if(fabs(value-1.0)>0.05)
            {
                if(value>1.0)
                {
                    qDebug() << "Step Up";
                    ui->SetValueSpinBox->stepUp();
                }
                else
                {
                    qDebug() << "Step Down";
                    ui->SetValueSpinBox->stepDown();
                }
            }
        }
    }

    if (gesture->state() == Qt::GestureFinished) {
        qDebug() << "Gesture Finished";

        if (false == m_CurrentBoxName.isEmpty()){
            QSudouBox *box = this->findChild<QSudouBox *>(m_CurrentBoxName);
            if (box != nullptr){
                box->Released();
            }
        }
    }
}

void QSudokuSolver::on_MakeButton_clicked()
{
    on_SolveButton_clicked();
}
