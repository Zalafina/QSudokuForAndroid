#ifndef QSUDOUBOX_H
#define QSUDOUBOX_H

#include <QLabel>
#include <QFocusEvent>
#include <QStylePainter>
#include <QStyleOptionFocusRect>

#include "qsudokusolver.h"

class QSudouBox : public QLabel
{
public:
    explicit QSudouBox(QWidget *parent = nullptr);

    enum BoxType
    {
        BOXTYPE_BLANK = 0U,
        BOXTYPE_PUZZLE,
        BOXTYPE_FILL,
    };
    Q_ENUM(BoxType)

    void clearall(void);
    void setpuzzlenumber(void);

    void Pressed(void);
    void Released(void);

    void setNormalTextColor(void);
    void setPuzzleTextColor(void);

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    //void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *keyevent);

public:
    quint32 m_BoxType;

private:
    QSudokuSolver *parent_ptr;
};

#endif // QSUDOUBOX_H
