#include <QDebug>
#include "qsudoubox.h"
#include "ui_qsudokusolver.h"

QSudouBox::QSudouBox(QWidget *parent) : QLabel(parent)
  , m_BoxType(BOXTYPE_BLANK)
  , parent_ptr(dynamic_cast<QSudokuSolver*>(parent))
{
    this->setFocusPolicy(Qt::NoFocus);
    setNormalTextColor();
}

void QSudouBox::clearall()
{
    this->clear();
    this->setFrameShape(QFrame::StyledPanel);
    this->setFrameShadow(QFrame::Plain);
    this->setStyleSheet("background-color:");
    this->m_BoxType = BOXTYPE_BLANK;
    setNormalTextColor();
}

void QSudouBox::setpuzzlenumber(void)
{
    this->setFrameShape(QFrame::WinPanel);
    this->setFrameShadow(QFrame::Sunken);
    this->setStyleSheet("background-color: orchid");
    this->m_BoxType = BOXTYPE_PUZZLE;
    setPuzzleTextColor();
}

void QSudouBox::Pressed()
{
    this->setFrameShape(QFrame::WinPanel);
    this->setFrameShadow(QFrame::Sunken);
    this->setStyleSheet("background-color: lightsteelblue");

    QSpinBox *valuespinbox = parent_ptr->GetUI()->SetValueSpinBox;
    if (false == this->text().isEmpty()){
        valuespinbox->setValue(this->text().toInt());
    }
    else{
        valuespinbox->clear();
    }

    QRect currentBoxGeometry = this->geometry();
    QRect setvalueGeometry = valuespinbox->geometry();
    int y = currentBoxGeometry.y() - valuespinbox->height();
    setvalueGeometry.setY(y);
    setvalueGeometry.setX(this->x());
    setvalueGeometry.setHeight(valuespinbox->height());
    setvalueGeometry.setWidth(valuespinbox->width());
    valuespinbox->setGeometry(setvalueGeometry);
    valuespinbox->setVisible(true);
}

void QSudouBox::Released()
{
    QSpinBox *valuespinbox = parent_ptr->GetUI()->SetValueSpinBox;
    if (true == valuespinbox->isVisible()){
        this->setText(valuespinbox->text());
    }

    if (QSudokuSolver::SUDOKUMODE_PLAY == parent_ptr->m_SudokuMode){
        if (true == this->text().isEmpty()){
            this->setFrameShape(QFrame::StyledPanel);
            this->setFrameShadow(QFrame::Plain);
            this->setStyleSheet("background-color:");

            this->m_BoxType = BOXTYPE_BLANK;
        }
        else{
            if (((0 == parent_ptr->GetUI()->PuzzleComboBox->currentIndex())
                 &&(true == parent_ptr->m_CustomPuzzleMaked))
                    ||(0 != parent_ptr->GetUI()->PuzzleComboBox->currentIndex())){
                this->setFrameShape(QFrame::StyledPanel);
                this->setFrameShadow(QFrame::Plain);
                this->setStyleSheet("background-color:");

                this->m_BoxType = BOXTYPE_FILL;
            }
            else{
                this->setStyleSheet("background-color: orchid");
                this->m_BoxType = BOXTYPE_PUZZLE;
            }
        }
    }
    else{ /* QSudokuSolver::SUDOKUMODE_SOLVE == QSudokuSolver::m_SudokuMode */
        if (true == this->text().isEmpty()){
            this->setFrameShape(QFrame::StyledPanel);
            this->setFrameShadow(QFrame::Plain);
            this->setStyleSheet("background-color:");
            this->m_BoxType = BOXTYPE_BLANK;
        }
        else{
            this->setStyleSheet("background-color: orchid");
            this->m_BoxType = BOXTYPE_PUZZLE;
        }
    }

    valuespinbox->setVisible(false);
    valuespinbox->clear();
}

void QSudouBox::setNormalTextColor()
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, QColor("royalblue"));
    this->setPalette(palette);
}

void QSudouBox::setPuzzleTextColor()
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, QColor("slategray"));
    this->setPalette(palette);
}

void QSudouBox::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    bool pressable = false;
    if ((QSudokuSolver::SUDOKUMODE_PLAY == parent_ptr->m_SudokuMode)
            && (QSudokuSolver::PUZZLE_CUSTOM == parent_ptr->GetUI()->PuzzleComboBox->currentIndex())
            && (false == parent_ptr->m_CustomPuzzleMaked)){
        pressable = true;
    }
    else if ((QSudokuSolver::SUDOKUMODE_SOLVE == parent_ptr->m_SudokuMode)
             && (QSudokuSolver::PUZZLE_CUSTOM == parent_ptr->GetUI()->PuzzleComboBox->currentIndex())
             && (false == parent_ptr->m_SolvedStatus)){
        pressable = true;
    }
    else if ((QSudokuSolver::PUZZLE_CUSTOM != parent_ptr->GetUI()->PuzzleComboBox->currentIndex())
             && (this->m_BoxType != BOXTYPE_PUZZLE)){
        if (QSudokuSolver::SUDOKUMODE_PLAY == parent_ptr->m_SudokuMode){
            pressable = true;
        }
    }


    if (true == pressable){
    #ifdef DEBUG_LOGOUT_ON
        //qDebug() << "mousePressEvent:" << this->objectName();
    #endif
        parent_ptr->boxPressed(this->objectName());
        Pressed();
    }
}

void QSudouBox::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    bool releaseable = false;
    if ((QSudokuSolver::SUDOKUMODE_PLAY == parent_ptr->m_SudokuMode)
            && (QSudokuSolver::PUZZLE_CUSTOM == parent_ptr->GetUI()->PuzzleComboBox->currentIndex())
            && (false == parent_ptr->m_CustomPuzzleMaked)){
        releaseable = true;
    }
    else if ((QSudokuSolver::SUDOKUMODE_SOLVE == parent_ptr->m_SudokuMode)
             && (QSudokuSolver::PUZZLE_CUSTOM == parent_ptr->GetUI()->PuzzleComboBox->currentIndex())
             && (false == parent_ptr->m_SolvedStatus)){
        releaseable = true;
    }
    else if ((QSudokuSolver::PUZZLE_CUSTOM != parent_ptr->GetUI()->PuzzleComboBox->currentIndex())
             && (this->m_BoxType != BOXTYPE_PUZZLE)){
        if (QSudokuSolver::SUDOKUMODE_PLAY == parent_ptr->m_SudokuMode){
            releaseable = true;
        }
    }

    if (true == releaseable){
    #ifdef DEBUG_LOGOUT_ON
        //qDebug() << "mouseReleaseEvent:" << this->objectName();
    #endif
        parent_ptr->boxReleased(this->objectName());
        Released();
    }
}

#if 0
void QSudouBox::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

//    QPainter painter(this);
//    QStyleOptionFocusRect option;
//    option.initFrom(this);
//    style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);

//    this->update();
}
#endif

void QSudouBox::keyPressEvent(QKeyEvent *keyevent)
{
    int keycode = keyevent->key();
    int keyvalue;
    QString str_key;

    switch (keycode)
    {
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        keyvalue = keycode - Qt::Key_0;
        str_key.setNum(keyvalue);
        this->setText(str_key);
        break;
    case Qt::Key_0:
    case Qt::Key_Delete:
        this->clear();
    default:
        break;
    }

}
