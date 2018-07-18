#include "mydockwidget.h"
#include "QPlainTextEdit"
#include "QDebug"
#include "QVBoxLayout"
#include "QLayout"
#include "myplaintextedit.h"
#include "QPainter"
#include "QToolTip"
#include "QScrollBar"

MyDockWidget::MyDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
{
    QDockWidget(title, parent, flags);
    textEdit = new MyPlainTextEdit;
    textEdit->setFrameStyle(QFrame::Raised|QFrame::Box);
    textEdit->MySetViewportMargins(14,0,0,0);
    textEdit->setContentsMargins(0,0,0,0);
    setWidget(textEdit);
    setWindowTitle(title);
    this->setAutoFillBackground(true);
    textEdit->setReadOnly(true);

    QPalette palette = this->palette();
    palette.setBrush(QPalette::WindowText, Qt::darkGray);
    this->setPalette(palette);
    setMouseTracking(true);
    connect(textEdit->verticalScrollBar(), &QScrollBar::valueChanged, this, &MyDockWidget::updateLeftColumnArea);
    connect(textEdit, &MyPlainTextEdit::blockDoubleClick, this, &MyDockWidget::getDoubleClickBlockText);
}
void MyDockWidget::updateLeftColumnArea(int value)
{
    this->repaint();
    textEdit->viewport()->repaint();
}
void MyDockWidget::updateTextArea(QList<QString> result)
{
    QTextBlockFormat blockFormat = textEdit->textDocument()->firstBlock().blockFormat();
    textEdit->clear();
    /*if(!result.resultPath.isEmpty())
    {
        QTextCursor cursor = textEdit->textCursor();
        QTextBlockFormat blockFormat;
        blockFormat.setBackground(QColor("#00FFFF"));
        QTextCharFormat charFormat = textEdit->currentCharFormat();
        charFormat.setForeground(QColor("#000000"));
        charFormat.setProperty(QTextFormat::FullWidthSelection,true);
        cursor.setBlockFormat(blockFormat);
        cursor.setCharFormat(charFormat);
        cursor.insertText(package.resultString);
        for(int i = 0; i<package.resultPath.size();i++)
        {
            blockFormat.setBackground(QColor("#FFEFD5"));
            charFormat.setForeground(QColor("#000000"));
            charFormat.setProperty(QTextFormat::FullWidthSelection,true);
            cursor.setCharFormat(charFormat);
            cursor.insertBlock(blockFormat);
            cursor.insertText("  " + package.resultPath.at(i));
            QString path = package.resultPath.at(i);
            QString filename = path.right(path.length() - (path.lastIndexOf("\\") + 1));
            QMap<QString, QList<QString> > detail = package.resultDetail;
            if(detail.contains(filename))
            {
                QList<QList<QString>> list = detail.values(filename);
                for(int j = 0; j< list.at(0).size(); j++)
                {
                    QString str = list.at(0).at(j);
                    QString lineNo = str.left(str.indexOf(QChar('@')));
                    QString line = str.right(str.length() - str.indexOf(QChar('@'))-1);
                    blockFormat.setBackground(QColor("#FFFFFF"));
                    charFormat.setForeground(QColor("#000000"));
                    cursor.insertBlock(blockFormat);
                    cursor.setCharFormat(charFormat);
                    cursor.insertText("    Line " + lineNo + ": " + line);
                }
            }
        }
    }*/
    for(int i = 0; i < result.size(); i++)
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

}
void MyDockWidget::updateTextArea()
{
    updateTextArea(currentSearchResult);
}
void MyDockWidget::setResult(QList<QString> result)
{
    searchResult = result;
    currentSearchResult = result;
}
void MyDockWidget::paintEvent(QPaintEvent *event)
{
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
    QDockWidget::mousePressEvent(event);
    currentSearchResult.clear();
    QList<QString> tempResult;
    for(int i = 0; i<viewholders.size(); i++)
    {
        int eventX = event->x();
        int eventY = event->y();
        if(viewholders.at(i).top < eventY &&eventY <viewholders.at(i).bottom
                && 0< eventX && eventX < 14)
        {
            QTextBlock block = textEdit->textDocument()->firstBlock();

            while(block.isValid())
            {
                QRectF rectf = textEdit->getblockBoundingGeometry(block);

                if(!this->isFloating())
                {
                    rectf.setTop(rectf.top() + 22);
                    rectf.setBottom(rectf.bottom() + 22);
                }
                if(rectf.top() == viewholders.at(i).top
                        && (rectf.bottom() == viewholders.at(i).bottom
                            ||rectf.bottom()-4 == viewholders.at(i).bottom) )
                {
                    if(block.text().at(0) == QChar('S')
                            && viewholders.at(i).plusInd == true)
                    {
                        updateTextArea(plusCaseSearchResult);
                        currentSearchResult = plusCaseSearchResult;
                        break;
                    }
                    else if(block.text().at(0) == QChar('S')
                            && viewholders.at(i).plusInd == false)
                    {
                        //keep block info into temp result package before click -
                        //to restore previous status.
                        plusCaseSearchResult.clear();
                        QTextBlock tempblock = textEdit->textDocument()->firstBlock();

                        while(tempblock.isValid())
                        {
                            /*QString resultPath;
                            resultPath.clear();
                            if(tempblock.text().at(0) == QChar('S') )
                            {
                                plusCaseSearchResult.resultString = tempblock.text();
                                tempblock = tempblock.next();
                            }
                            else if(tempblock.text().left(4) == QString("    "))
                            {
                                QList<QString> tempDetailList;
                                while(tempblock.text().left(4) == QString("    "))
                                {
                                    tempDetailList.append(tempblock.text());
                                    tempblock = tempblock.next();
                                }
                                plusCaseSearchResult.resultDetail.insert(resultPath, tempDetailList);
                            }
                            else if(tempblock.text().left(2) == QString("  "))
                            {
                                //resultPath is kept to insert QMap<QString, QList<QString>> above
                                resultPath = tempblock.text();
                                resultPathTemp.append(tempblock.text());
                                tempblock = tempblock.next();
                            }*/

                            plusCaseSearchResult.append(tempblock.text());
                            tempblock = tempblock.next();
                        }

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
                            QTextBlock tempBlock = textEdit->textDocument()->firstBlock();
                            //store the block from start.
                            while(block != tempBlock)
                            {
                                tempResult.append(tempBlock.text());
                                tempBlock = tempBlock.next();
                            }
                            //append current file path into result.
                            tempResult.append(tempBlock.text());
                            //append detail result from input source under this file.
                            int index = searchResult.indexOf(block.text());
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
                            tempBlock = tempBlock.next();
                            while(tempBlock.isValid())
                            {
                                tempResult.append(tempBlock.text());
                                tempBlock = tempBlock.next();
                            }
                            updateTextArea(tempResult);
                            currentSearchResult = tempResult;
                            break;
                        }
                        else if(viewholders.at(i).plusInd == false)
                        {
                            QTextBlock tempBlock = textEdit->textDocument()->firstBlock();
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

                            //continue append rest of block
                            while(tempBlock.isValid())
                            {
                                tempResult.append(tempBlock.text());
                                tempBlock = tempBlock.next();
                            }
                            updateTextArea(tempResult);
                            currentSearchResult = tempResult;

                            /*
                            QTextBlock tempBlock = textEdit->document()->firstBlock();
                            currentSearchResult.append(tempBlock.text());
                            while(block != tempBlock)
                            {
                                tempBlock = tempBlock.next();
                                currentSearchResult.append(tempBlock.text());
                            }

                            tempBlock = tempBlock.next();
                            while(tempBlock.text().left(4) == QString("    ")&&tempBlock.isValid())
                            {
                                tempBlock.setVisible(false);
                                tempBlock = tempBlock.next();
                            }*/
                            break;
                        }
                    }
                }
                else
                {
                    block = block.next();
                }
            }
            break;
        }
    }
    this->repaint();
    textEdit->viewport()->repaint();
}

void MyDockWidget::getDoubleClickBlockText(QString text)
{
    doubleClickBlockText = text;
    QList<QString> tempResult;
    QTextBlock block = textEdit->textDocument()->firstBlock();

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
                    QTextBlock tempblock = textEdit->textDocument()->firstBlock();

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
