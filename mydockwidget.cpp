#include "mydockwidget.h"
#include "QPlainTextEdit"
#include "QDebug"
#include "QVBoxLayout"
#include "QLayout"
#include "myplaintextedit.h"
#include "QPainter"
#include "QToolTip"
#include "QScrollBar"
#include "QDockWidget"
MyDockWidget::MyDockWidget(QString title, QWidget *parent):QDockWidget(parent)
{
    qDebug()<<"MyDockWidget::MyDockWidget";
    textEdit = new MyPlainTextEdit;
    //textEdit->setFrameStyle(QFrame::Raised|QFrame::Box);
    textEdit->MySetViewportMargins(14,0,0,0);
    textEdit->setContentsMargins(0,0,0,0);
    setWidget(textEdit);
    setWindowTitle(title);
    textEdit->setReadOnly(true);

    /*QPalette palette = this->palette();
    palette.setBrush(QPalette::WindowText, Qt::darkGray);
    this->setPalette(palette);*/
    setMouseTracking(true);
    connect(textEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &MyDockWidget::updateLeftColumnArea);
    //connect(textEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &MyDockWidget::paintLine);
    connect(textEdit, &MyPlainTextEdit::blockDoubleClick, this, &MyDockWidget::getDoubleClickBlockText);
    connect(textEdit, &MyPlainTextEdit::cursorPositionChanged, this, &MyDockWidget::highlightCurrentLine);
}
void MyDockWidget::updateLeftColumnArea(int value)
{
    qDebug()<<"MyDockWidget::updateLeftColumnArea";
    this->repaint();
    textEdit->viewport()->repaint();
}
void MyDockWidget::updateTextArea(QList<QString> result)
{
    qDebug()<<"MyDockWidget::updateTextArea(QList<QString>)";
    disconnect(textEdit, &MyPlainTextEdit::cursorPositionChanged, this, &MyDockWidget::highlightCurrentLine);
    QTextBlockFormat blockFormat = textEdit->document()->firstBlock().blockFormat();
    textEdit->clear();
    for(int i = 0; i<result.size(); i++)
    {
        QTextCursor cursor = textEdit->textCursor();
        QTextCharFormat charFormat = textEdit->currentCharFormat();

        if(result.at(i).at(0) == QChar('S'))
        {
            blockFormat.setBackground(QColor("#00FFFF"));
            charFormat.setForeground(QColor("#000000"));
            //charFormat.setProperty(QTextFormat::FullWidthSelection,true);
            cursor.setBlockFormat(blockFormat);
            cursor.setCharFormat(charFormat);
            cursor.insertText(result.at(i));
        }
        else if(result.at(i).left(4) == QString("    "))
        {
            blockFormat.setBackground(QColor("#FFFFFF"));
            charFormat.setForeground(QColor("#000000"));
            cursor.insertBlock(blockFormat);
            cursor.setCharFormat(charFormat);
            cursor.insertText(result.at(i));
        }
        else if(result.at(i).left(2) == QString("  "))
        {
            blockFormat.setBackground(QColor("#FFEFD5"));
            charFormat.setForeground(QColor("#000000"));
            charFormat.setProperty(QTextFormat::FullWidthSelection,true);
            cursor.insertBlock(blockFormat);
            cursor.setCharFormat(charFormat);
            cursor.insertText(result.at(i));
        }
    }
    markSearchString();

    //move to top of textEdit to display 1st line or else display last line.
    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition(0);
    textEdit->setTextCursor(cursor);
    connect(textEdit, &MyPlainTextEdit::cursorPositionChanged, this, &MyDockWidget::highlightCurrentLine);
}
void MyDockWidget::updateTextArea()
{
    qDebug()<<"MyDockWidget::updateTextArea()";
    updateTextArea(currentSearchResult);
}
void MyDockWidget::setResult(QList<QString> result)
{
    qDebug()<<"MyDockWidget::setResult";
    searchResult = result;
    currentSearchResult = result;
}
void MyDockWidget::paintEvent(QPaintEvent *event)
{
    qDebug()<<"MyDockWidget::paintEvent";
    QDockWidget::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::black);
    viewholders.clear();
    lineCordinateList.clear();

    int ContentWidth = textEdit->rect().width()-22;
    int offSet = textEdit->getContentOffSet().y();
    int fontHeight = textEdit->fontMetrics().height();
    int fixWidth = 14;
    int top = 22;

    viewHolder holder;
    QTextBlock block = textEdit->getFirstVisiableBlock();

    while(!block.text().isEmpty()&&block.isValid())
    {
        QString firstLineText = block.text();
        QString NextLineText = block.next().text();

        if(firstLineText.at(0) == QChar('S')
                && NextLineText.left(2) == QString("  ")
                && !NextLineText.isEmpty())
        {
            if(this->isFloating())
            {
                painter.drawText(QRect(0,offSet,fixWidth,fontHeight),
                                 Qt::AlignCenter,
                                 "-");
                holder.top = offSet;
                holder.bottom = offSet + fontHeight;
                holder.plusInd = false;
                viewholders.append(holder);
            }
            else
            {
                painter.drawText(QRect(0,top + offSet,fixWidth,fontHeight),
                                 Qt::AlignCenter,
                                 "-");
                holder.top = top + offSet;
                holder.bottom = top + offSet + fontHeight;
                holder.plusInd = false;
                viewholders.append(holder);
            }
        }
        else if(firstLineText.at(0) == QChar('S')
                && NextLineText.isEmpty())
        {
            if(this->isFloating())
            {
                painter.drawText(QRect(0,offSet,fixWidth,fontHeight),
                                 Qt::AlignCenter,
                                 "+");

                holder.top = offSet;
                holder.bottom = offSet + fontHeight;
                holder.plusInd = true;
                viewholders.append(holder);
            }
            else
            {
                painter.drawText(QRect(0,top + offSet,fixWidth,fontHeight),
                                 Qt::AlignCenter,
                                 "+");

                holder.top = top + offSet;
                holder.bottom = top + offSet + fontHeight;
                holder.plusInd = true;
                viewholders.append(holder);
            }
            QString points;
            points = QString::number(0) + QString("@")
                    + QString::number(top + offSet + fontHeight) + QString("@")
                    + QString::number(ContentWidth) + QString("@")
                    + QString::number(top + offSet + fontHeight);
            lineCordinateList.append(points);
        }
        else if(firstLineText.left(2) == QString("  ")
                &&firstLineText.at(2) != QChar(' ')
                &&!NextLineText.isEmpty()
                &&NextLineText.at(2) == QChar(' '))
        {
            if(this->isFloating())
            {
                painter.drawText(QRect(0,top + offSet-22,fixWidth,fontHeight),
                                 Qt::AlignCenter,
                                 "-");
                holder.top = top + offSet-22;
                holder.bottom = top + offSet + fontHeight-22;
                holder.plusInd = false;
                viewholders.append(holder);
            }
            else
            {
                painter.drawText(QRect(0,top + offSet,fixWidth,fontHeight),
                                 Qt::AlignCenter,
                                 "-");
                holder.top = top + offSet;
                holder.bottom = top + offSet + fontHeight;
                holder.plusInd = false;
                viewholders.append(holder);
            }
        }
        else if(firstLineText.left(2) == QString("  ")
                &&firstLineText.at(2) != QChar(' ')
                &&((!NextLineText.isEmpty()
                &&NextLineText.at(2) != QChar(' '))
                || NextLineText.isEmpty()))
        {
            if(this->isFloating())
            {
                painter.drawText(QRect(0,top + offSet-22,fixWidth,fontHeight),
                                 Qt::AlignCenter,
                                 "+");
                holder.top = top + offSet -22;
                holder.bottom = top + offSet + fontHeight -22;
                holder.plusInd = true;
                viewholders.append(holder);
            }
            else
            {
                painter.drawText(QRect(0,top + offSet,fixWidth,fontHeight),
                                 Qt::AlignCenter,
                                 "+");
                holder.top = top + offSet;
                holder.bottom = top + offSet + fontHeight;
                holder.plusInd = true;
                viewholders.append(holder);
            }

            QString points;
            points = QString::number(0) + QString("@")
                    + QString::number(top + offSet + fontHeight) + QString("@")
                    + QString::number(ContentWidth) + QString("@")
                    + QString::number(top + offSet + fontHeight);
            lineCordinateList.append(points);
        }
        top = top + fontHeight;
        block = block.next();
        textEdit->paintLine(lineCordinateList);
    }
}

void MyDockWidget::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<"MyDockWidget::mouseMoveEvent";
    QDockWidget::mouseMoveEvent(event);
    QCursor cursor = this->cursor();
    if(event->x()<= 14 && event->y()<= this->rect().height())
    {
        cursor.setShape(Qt::PointingHandCursor);
        this->setCursor(cursor);
    }
    else
    {
        cursor.setShape(Qt::ArrowCursor);
        this->setCursor(cursor);
    }
}

void MyDockWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"MyDockWidget::mousePressEvent";
    QDockWidget::mousePressEvent(event);
    currentSearchResult.clear();
    QList<QString> tempResult;
    qDebug()<<"viewholders.size() = "<<viewholders.size();
    for(int i = 0; i<viewholders.size(); i++)
    {
        int eventX = event->x();
        int eventY = event->y();
        if(viewholders.at(i).top < eventY &&eventY <viewholders.at(i).bottom
                && 0< eventX && eventX < 14)
        {
            QTextBlock block = textEdit->document()->firstBlock();
            qDebug()<<"block text = "<<block.text();
            while(block.isValid())
            {
                QRectF rectf = textEdit->getblockBoundingGeometry(block);
                qDebug()<<1;
                if(!this->isFloating())
                {
                    rectf.setTop(rectf.top() + 22);
                    rectf.setBottom(rectf.bottom() + 22);
                }
                qDebug()<<2;
                if(rectf.top() == viewholders.at(i).top
                        && (rectf.bottom() == viewholders.at(i).bottom
                            ||rectf.bottom()-4 == viewholders.at(i).bottom) )
                {
                    if(block.text().at(0) == QChar('S')
                            && viewholders.at(i).plusInd == true)
                    {
                        qDebug()<<3;
                        updateTextArea(plusCaseSearchResult);
                        currentSearchResult = plusCaseSearchResult;
                        break;
                    }
                    else if(block.text().at(0) == QChar('S')
                            && viewholders.at(i).plusInd == false)
                    {
                        //keep block info into temp result package before click -
                        //to restore previous status.
                        qDebug()<<4;
                        plusCaseSearchResult.clear();
                        QTextBlock tempblock = textEdit->document()->firstBlock();

                        while(tempblock.isValid())
                        {
                            plusCaseSearchResult.append(tempblock.text());
                            tempblock = tempblock.next();
                        }
                        qDebug()<<5;
                        tempResult.append(searchResult.at(0));
                        updateTextArea(tempResult);
                        currentSearchResult = tempResult;
                        break;
                    }
                    else if(block.text().left(2) == QString("  ")
                            &&block.text().left(4) != QString("    "))
                    {
                        if(viewholders.at(i).plusInd == true)
                        {
                            qDebug()<<6;
                            QTextBlock tempBlock = textEdit->document()->firstBlock();
                            //store the block from start.
                            while(block != tempBlock)
                            {
                                tempResult.append(tempBlock.text());
                                tempBlock = tempBlock.next();
                            }
                            //append current file path into result.
                            tempResult.append(tempBlock.text());
                            qDebug()<<tempResult;
                            //append detail result from input source under this file.
                            int index = searchResult.indexOf(block.text());
                            qDebug()<<"index = "<<index;
                            if(index != -1)
                            {
                                index++;
                            }
                            qDebug()<<7;
                            qDebug()<<searchResult.size();
                            qDebug()<<searchResult;
                            if(index < searchResult.size() && index >=0)
                            {
                                while(index < searchResult.size()
                                      &&searchResult.at(index).left(4) == QString("    "))
                                {
                                    tempResult.append(searchResult.at(index));
                                    index++;
                                    if(index >= searchResult.size())
                                        break;
                                }
                            }
                            qDebug()<<tempResult;
                            qDebug()<<8;
                            //continue append rest of result in next file if any.
                            tempBlock = tempBlock.next();
                            while(tempBlock.isValid())
                            {
                                tempResult.append(tempBlock.text());
                                tempBlock = tempBlock.next();
                            }
                            qDebug()<<tempResult;
                            qDebug()<<8.5;
                            updateTextArea(tempResult);
                            currentSearchResult = tempResult;
                            break;
                        }
                        else if(viewholders.at(i).plusInd == false)
                        {
                            qDebug()<<9;
                            QTextBlock tempBlock = textEdit->document()->firstBlock();
                            //store the block from start.
                            while(block != tempBlock)
                            {
                                tempResult.append(tempBlock.text());
                                tempBlock = tempBlock.next();
                            }
                            //append current file path into result.
                            tempResult.append(tempBlock.text());

                            tempBlock = tempBlock.next();
                            //skip detail result in current file.
                            while(tempBlock.text().left(4) == QString("    ")&&tempBlock.isValid())
                            {
                                tempBlock = tempBlock.next();
                            }
                            qDebug()<<10;
                            //continue append rest of block
                            while(tempBlock.isValid())
                            {
                                tempResult.append(tempBlock.text());
                                tempBlock = tempBlock.next();
                            }
                            updateTextArea(tempResult);
                            currentSearchResult = tempResult;
                            break;
                        }
                    }
                }
                else
                {
                    qDebug()<<11;
                    block = block.next();
                }
            }
            break;
        }
    }
    qDebug()<<12;
    this->repaint();
    textEdit->viewport()->repaint();
}

void MyDockWidget::getDoubleClickBlockText(QString text)
{
    qDebug()<<"MyDockWidget::getDoubleClickBlockText";
    doubleClickBlockText = text;
    QList<QString> tempResult;
    QTextBlock block = textEdit->document()->firstBlock();
    if(text.left(4) != QString("    "))
    {
        while(block.isValid())
        {
            if(block.text() != text)
            {
                tempResult.append(block.text());
                block = block.next();
            }
            else
            {
                tempResult.append(block.text());
                if(block.next().isValid())
                {
                    QString nextBlockText = block.next().text();

                    if(block.text().at(0) == QChar('S')
                            &&nextBlockText.isEmpty())
                    {
                        updateTextArea(plusCaseSearchResult);
                        currentSearchResult = plusCaseSearchResult;
                        break;
                    }
                    else if(block.text().at(0) == QChar('S')
                            &&!nextBlockText.isEmpty())
                    {
                        plusCaseSearchResult.clear();
                        QTextBlock tempblock = textEdit->document()->firstBlock();

                        while(tempblock.isValid())
                        {
                            plusCaseSearchResult.append(tempblock.text());
                            tempblock = tempblock.next();
                        }
                        updateTextArea(tempResult);
                        currentSearchResult = tempResult;
                        break;
                    }
                    else if(block.text().left(2) == QString("  ")
                            &&block.text().left(4) != QString("    ")
                            &&nextBlockText.left(4) == QString("    "))
                    {
                        block = block.next();
                        while(block.isValid()&&block.text().left(4) == QString("    "))
                        {
                            block = block.next();
                        }
                        while(block.isValid())
                        {
                            tempResult.append(block.text());
                            block = block.next();
                        }
                        updateTextArea(tempResult);
                        currentSearchResult = tempResult;
                        break;
                    }
                    else if(block.text().left(2) == QString("  ")
                            &&nextBlockText.left(2) == QString("  ")
                            &&nextBlockText.left(4) != QString("    "))
                    {
                        int index = searchResult.indexOf(doubleClickBlockText);
                        if(index != -1)
                        {
                            index++;
                        }
                        if(index < searchResult.size() && index >=0)
                        {
                            while(index < searchResult.size()
                                  &&searchResult.at(index).left(4) == QString("    "))
                            {
                                tempResult.append(searchResult.at(index));
                                index++;
                                if(index >= searchResult.size())
                                    break;
                            }
                        }
                        //continue append rest of result in next file if any.
                        block = block.next();
                        while(block.isValid())
                        {
                            tempResult.append(block.text());
                            block = block.next();
                        }
                        updateTextArea(tempResult);
                        currentSearchResult = tempResult;
                        break;
                    }
                    else
                    {
                        block = block.next();
                    }

                }
                else
                {
                    if(block.text().at(0) == QChar('S'))
                    {
                        updateTextArea(plusCaseSearchResult);
                        currentSearchResult = plusCaseSearchResult;
                        break;
                    }
                    else if(block.text().left(2) == QString("  "))
                    {
                        int index = searchResult.indexOf(doubleClickBlockText);
                        if(index != -1)
                        {
                            index++;
                        }
                        if(index < searchResult.size() && index >=0)
                        {
                            while(index < searchResult.size()
                                  &&searchResult.at(index).left(4) == QString("    "))
                            {
                                tempResult.append(searchResult.at(index));
                                index++;
                                if(index >= searchResult.size())
                                    break;
                            }
                        }

                        updateTextArea(tempResult);
                        currentSearchResult = tempResult;
                        break;
                    }
                }
            }
        }
        this->repaint();
        textEdit->viewport()->repaint();
    }
    else
    {
        QList<QTextEdit::ExtraSelection> extraSelections = textEdit->extraSelections();
        QTextEdit::ExtraSelection extraSelection;
        extraSelection.cursor = textEdit->textCursor();
        extraSelection.format = textEdit->textCursor().charFormat();
        extraSelection.format.setBackground(Qt::yellow);
        extraSelection.format.setForeground(Qt::black);
        extraSelection.format.setProperty(QTextFormat::FullWidthSelection,true);
        extraSelections.append(extraSelection);
        textEdit->setExtraSelections(extraSelections);
    }
}

void MyDockWidget::highlightCurrentLine()
{
    qDebug()<<"MyDockWidget::highlightCurrentLine()start";
    int blockNumber = textEdit->textCursor().blockNumber();
    paintLine();
    QTextBlock block = textEdit->document()->findBlockByNumber(blockNumber);

    disconnect(textEdit, &MyPlainTextEdit::cursorPositionChanged, this, &MyDockWidget::highlightCurrentLine);
    textEdit->setTextCursor(QTextCursor(block));
    connect(textEdit, &MyPlainTextEdit::cursorPositionChanged, this, &MyDockWidget::highlightCurrentLine);

    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection extraSelection;
    extraSelection.cursor = textEdit->textCursor();
    extraSelection.format = textEdit->textCursor().charFormat();
    extraSelection.format.setBackground(QColor(200,255,255,255));
    extraSelection.format.setForeground(Qt::black);
    extraSelection.format.setProperty(QTextFormat::FullWidthSelection,true);
    extraSelections.append(extraSelection);
    textEdit->setExtraSelections(extraSelections);

    markSearchString();
    qDebug()<<"MyDockWidget::highlightCurrentLine end";
}

void MyDockWidget::paintLine()
{
    qDebug()<<"MyDockWidget::paintLine start";
    disconnect(textEdit, &MyPlainTextEdit::cursorPositionChanged, this, &MyDockWidget::highlightCurrentLine);
    QTextBlock block = textEdit->getFirstVisiableBlock();
    QTextBlockFormat blockFormat = block.blockFormat();
    QTextCharFormat charFormat = block.charFormat();

    QRect rect = textEdit->viewport()->rect();
    int fontHeight = textEdit->fontMetrics().height();

    int start = 0;
    QString tempText;
    while(!block.text().isEmpty()&&block.isValid() && start <= rect.height())
    {
        QTextCursor cursor(block);
        tempText = block.text();
        if(tempText.at(0) == QChar('S'))
        {
            cursor.select(QTextCursor::BlockUnderCursor);
            cursor.removeSelectedText();
            blockFormat.setBackground(QColor("#00FFFF"));
            charFormat.setForeground(QColor("#000000"));
            cursor.setBlockFormat(blockFormat);
            cursor.setCharFormat(charFormat);
            cursor.insertText(tempText);
        }
        else if(tempText.left(4) == QString("    "))
        {
            cursor.select(QTextCursor::BlockUnderCursor);
            cursor.removeSelectedText();
            blockFormat.setBackground(QColor("#FFFFFF"));
            charFormat.setForeground(QColor("#000000"));
            cursor.insertBlock(blockFormat);
            cursor.setCharFormat(charFormat);
            cursor.insertText(tempText);
        }
        else if(tempText.left(2) == QString("  "))
        {
            cursor.select(QTextCursor::BlockUnderCursor);
            cursor.removeSelectedText();
            blockFormat.setBackground(QColor("#FFEFD5"));
            charFormat.setForeground(QColor("#000000"));
            cursor.insertBlock(blockFormat);
            cursor.setCharFormat(charFormat);
            cursor.insertText(tempText);
        }
        block = block.next();
        start = start + fontHeight;
    }
    connect(textEdit, &MyPlainTextEdit::cursorPositionChanged, this, &MyDockWidget::highlightCurrentLine);

    qDebug()<<"MyDockWidget::paintLine end";
}
void MyDockWidget::markSearchString()
{
    qDebug()<<"MyDockWidget::markSearchString start";
    QTextBlock block = textEdit->document()->firstBlock();
    QList<QTextEdit::ExtraSelection> extraSelections = textEdit->extraSelections();
    searchString = searchResult.at(0);
    searchString = searchString.mid(8, searchString.indexOf('(')-8-2);

    QString LastBlockText = 0;
    int lastCursorPosition = 0;
    while(block.isValid())
    {
        QTextCursor cursor(block);
        //only mark content line
        if(block.text().left(4) == QString("    "))
        {
            //in case of one line contains more than one searchString.
            if(block.text() == LastBlockText)
            {
                cursor = textEdit->document()->find(searchString, cursor.position()+lastCursorPosition);
                QTextCharFormat colorFormat(cursor.charFormat());
                colorFormat.setBackground(Qt::yellow);
                QTextEdit::ExtraSelection extraSelection;
                extraSelection.cursor = cursor;
                extraSelection.format = colorFormat;
                extraSelections.append(extraSelection);

                //for next loop use
                lastCursorPosition = cursor.positionInBlock();
                LastBlockText = block.text();
                block = block.next();
            }
            else
            {
                cursor = textEdit->document()->find(searchString, cursor);
                QTextCharFormat colorFormat(cursor.charFormat());
                colorFormat.setBackground(Qt::yellow);
                QTextEdit::ExtraSelection extraSelection;
                extraSelection.cursor = cursor;
                extraSelection.format = colorFormat;
                extraSelections.append(extraSelection);

                //for next loop use
                lastCursorPosition = cursor.positionInBlock();
                LastBlockText = block.text();
                block = block.next();
            }
        }
        else
        {
            block = block.next();
        }
    }
    textEdit->setExtraSelections(extraSelections);
    qDebug()<<"MyDockWidget::markSearchString end";
}
