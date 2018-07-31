#include "myplaintextedit.h"
#include "QPlainTextEdit"
#include "QPainter"
#include "QDebug"
#include "QTextBlock"
MyPlainTextEdit::MyPlainTextEdit(QWidget *parent):QPlainTextEdit(parent)
{
    setViewportMargins(10,0,0,0);
    setContentsMargins(0,0,0,0);
    setMouseTracking(true);
    connect(this, SIGNAL(textChanged()), this, SLOT(update()));
}

void MyPlainTextEdit::MySetViewportMargins(const QMargins &margins)
{
    setViewportMargins(margins);
}

void MyPlainTextEdit::MySetViewportMargins(int left, int top, int right, int bottom)
{
    setViewportMargins(left, top, right, bottom);
}

QString MyPlainTextEdit::getFirstBlockInfo()
{
    currentBlock = firstVisibleBlock();
    return currentBlock.text();
}
QString MyPlainTextEdit::getNextBlockInfo()
{
    currentBlock = currentBlock.next();
    if(currentBlock.isValid()&&currentBlock.isVisible())
    {
        return currentBlock.text();
    }
    else
        return QString("");
}

QTextBlock MyPlainTextEdit::getFirstVisiableBlock()
{
    return firstVisibleBlock();
}

QPointF MyPlainTextEdit::getContentOffSet()
{
    return contentOffset();
}

void MyPlainTextEdit::paintLine(QList<QString> List)
{
    lineList = List;
}

void MyPlainTextEdit::paintEvent(QPaintEvent *e)
{
    QPlainTextEdit::paintEvent(e);
    QPainter painter(viewport());
    painter.setPen(Qt::black);
    int lastLineY = this->blockCount() * blockBoundingGeometry(document()->firstBlock()).height();

    for(int i = 0; i< lineList.size(); i++)
    {
        int flag = 0;
        QString startX = nullptr;
        QString startY = nullptr;
        QString endX = nullptr;
        QString endY = nullptr;
        QString temp = lineList.at(i);
        for(int j = 0; j<temp.length(); j++)
        {
            if(temp.at(j) != QChar('@') && flag == 0)
            {
                startX = startX + temp.at(j);
            }
            else if(temp.at(j) != QChar('@') && flag == 1)
            {
                startY = startY + temp.at(j);
            }
            else if(temp.at(j) != QChar('@') && flag == 2)
            {
                endX = endX + temp.at(j);
            }
            else if(temp.at(j) != QChar('@') && flag == 3)
            {
                endY = endY + temp.at(j);
            }
            else if(temp.at(j) == QChar('@'))
            {
                flag++;
            }
        }
        if( startY.toInt()-22 >= lastLineY)
        {
            painter.drawLine(QPoint(startX.toInt(),
                                    startY.toInt()-18),
                             QPoint(endX.toInt(),
                                    endY.toInt()-18));

        }
        else
        {
            painter.drawLine(QPoint(startX.toInt(),
                                    startY.toInt()-22),
                             QPoint(endX.toInt(),
                                    endY.toInt()-22));

        }
    }
}


QRectF MyPlainTextEdit::getblockBoundingGeometry(QTextBlock &block)
{
    return blockBoundingGeometry(block).translated(contentOffset());
}

void MyPlainTextEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPlainTextEdit::mouseDoubleClickEvent(event);
    QTextCursor cursor = this->textCursor();
    QTextBlock block = cursor.block();
    emit blockDoubleClick(block.text());
}
