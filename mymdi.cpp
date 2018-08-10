#include "mymdi.h"
#include "QMessageBox"
#include "QFileInfo"
#include "QFileDialog"
#include "QTextStream"
#include "QPlainTextEdit"
#include "QPushButton"
#include "QCloseEvent"
#include "QPrintDialog"
#include "QPainter"
#include "QPrinter"
#include "QTextBlock"
#include "QDebug"
MyMdi::MyMdi(QWidget *parent):QPlainTextEdit(parent)
{
    qDebug()<<"MyMdi::MyMdi()";
    lineNumberArea = new LineNumberArea(this);

    setAttribute(Qt::WA_DeleteOnClose);
    textDocument = this->document();

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, &MyMdi::textChanged, this,&MyMdi::fileModified);
    connect(this, &MyMdi::cursorPositionChanged, this, &MyMdi::setTextColor);
    //connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    setAccessibleName(CurrFileName);
    updateLineNumberWidth(0);
}
void MyMdi::highlightCurrentLine()
{
    qDebug()<<"MyMdi::highlightCurrentLine()";
    QList<QTextEdit::ExtraSelection> extraSelections = this->extraSelections();
    if(extraSelections.size() <= 1)
    {
        QTextEdit::ExtraSelection extraSelection;
        QTextCursor textcursor(textCursor());
        extraSelection.cursor = textcursor;
        extraSelection.cursor.clearSelection();
        extraSelection.format = textCursor().charFormat();
        extraSelection.format.setBackground(QColor(200,255,255,255));
        extraSelection.format.setForeground(Qt::black);
        extraSelection.format.setProperty(QTextFormat::FullWidthSelection,true);
        extraSelections.append(extraSelection);
        setExtraSelections(extraSelections);
        return;
    }
    for(int i = 0; i< extraSelections.size();i++)
    {
        if(extraSelections.at(i).cursor.blockNumber() == textCursor().blockNumber())
        {
            QTextEdit::ExtraSelection extraSelection;
            QTextCursor textcursor(textCursor());
            extraSelection.cursor = textcursor;
            extraSelection.cursor.clearSelection();
            extraSelection.format = textCursor().charFormat();
            extraSelection.format.setBackground(QColor(200,255,255,255));
            extraSelection.format.setProperty(QTextFormat::FullWidthSelection,true);
            extraSelections.insert(0,extraSelection);
            setExtraSelections(extraSelections);
            break;
        }
    }
}

int MyMdi::lineNumberWidth()
{
    //qDebug()<<"MyMdi::lineNumberWidth()";
    int digits = 1;
    int max = qMax(1, blockCount());
    while(max >= 10)
    {
        max /= 10;
        ++ digits;
    }
    int space =  10 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;

}
void MyMdi::updateLineNumberWidth(int newBlockCount)
{
    //qDebug()<<"MyMdi::updateLineNumberWidth()";
    newBlockCount = 0;
    setViewportMargins(lineNumberWidth(), 0 , 0,  0);
    this->repaint();
}
void MyMdi::updateLineNumberArea(const QRect &rect, int dy)
{
    //qDebug()<<"MyMdi::updateLineNumberArea()";
    if(dy)
    {
        lineNumberArea->scroll(0,dy);
    }
    else
    {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.x());
    }

    if(rect.contains(viewport()->rect()))
    {
        updateLineNumberWidth(0);
    }
}
void MyMdi::resizeEvent(QResizeEvent *e)
{
    //qDebug()<<"MyMdi::resizeEvent()";
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberWidth(),cr.height()));
}

void MyMdi::setSearchString()
{
    qDebug()<<"MyMdi::setSearchString()";
    searchString = textCursor().selectedText();
    Find(searchString, QTextDocument::FindWholeWords, true);
}

void MyMdi::setSearchStringFromTab(QString text)
{
    qDebug()<<"MyMdi::setSearchStringFromTab()";
    searchString  = text;
}
void MyMdi::lineNumberPaintEvent(QPaintEvent *event)
{
    //qDebug()<<"MyMdi::lineNumberPaintEvent()";
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor("#FFE4E1"));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = 0;

    if(blockNumber == 0)
    {
        bottom = (int)blockBoundingRect(block).height() + contentOffset().y();
    }
    else
    {
        bottom = (int)blockBoundingRect(block).height();
    }
    while(block.isValid() && top <= event->rect().bottom())
    {
        if(block.isVisible() && bottom>= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0,top, lineNumberArea->width(),
                             fontMetrics().height(),
                             Qt::AlignCenter,
                             number);
        }
        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        ++blockNumber;
    }
}
bool MyMdi::NewFile()
{
    qDebug()<<"MyMdi::NewFile()";
    IsUntitled = true;
    static int sequence_no = 1;
    CurrFileName = tr("Untitled_%1.txt").arg(sequence_no++);
    setWindowTitle(CurrFileName);
    setAccessibleName(CurrFileName);
    return true;
}
bool MyMdi::OpenFile( QString fileName)
{
    qDebug()<<"MyMdi::OpenFile()";
    if(fileName.isEmpty()&&fileName != QString("0"))
    {
        CurrFileName = QFileDialog::getOpenFileName(this);
        if(!CurrFileName.isEmpty())
        {
            return LoadFile(CurrFileName);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return LoadFile(fileName);
    }
}
bool MyMdi::LoadFile(QString filename)
{
    qDebug()<<"MyMdi::LoadFile() start";
    QFile in(filename);
    if(!in.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"),tr("Open File Error"));
        qDebug()<<in.errorString();
        return false;
    }
    QTextStream instream(&in);
    this->clear();
    this->appendPlainText(instream.readAll());
    CurrFileName = QFileInfo(in).fileName();
    CurrFilePath = QFileInfo(in).filePath();
    connect(this, &QPlainTextEdit::textChanged, this, &MyMdi::fileModified);
    setAccessibleName(CurrFileName);
    IsUntitled = false;
    IsFileSaved = true;
    setWindowTitle(CurrFileName);
    resetCursorPosition();
    return true;
    qDebug()<<"MyMdi::LoadFile() end";
}
bool MyMdi::Save()
{
    qDebug()<<"MyMdi::Save()";
    if(IsUntitled)
    {
        return SaveAs();
    }
    else
    {
        return SaveFile(CurrFileName);
    }
}
bool MyMdi::SaveAs()
{
    qDebug()<<"MyMdi::SaveAs()";
    CurrFileName = QFileDialog::getSaveFileName(this,
                                                tr("Save As"),
                                                CurrFilePath,
                                                tr("Text Document(*.txt)"));
    if(!CurrFileName.isEmpty())
    {
        return SaveFile(CurrFileName);
    }
    else
    {
        return false;
    }
}
bool MyMdi::CopySaveAs()
{
    qDebug()<<"MyMdi::CopySaveAs()";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    CurrFilePath,
                                                    tr("Text Document(*.txt)"));
    if(!fileName.isEmpty())
    {
        QFile out(fileName);
        if(!out.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Error"), tr("Save File Error"));
            qDebug()<<out.errorString();
            return false;
        }
        QTextStream outStream(&out);
        outStream<<this->toPlainText();
        out.close();
        return true;
    }
    else
    {
        return false;
    }
}
bool MyMdi::SaveFile(QString fileName)
{
    qDebug()<<"MyMdi::SaveFile()";
    QFile out(fileName);
    if(!out.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), tr("Save File Error"));
        qDebug()<<out.errorString();
        return false;
    }
    QTextStream outStream(&out);
    outStream<<this->toPlainText();
    setWindowTitle(fileName);
    CurrFileName = QFileInfo(out).fileName();
    setAccessibleName(CurrFileName);
    IsUntitled = false;
    IsFileSaved = true;
    out.close();
    return true;
}
void MyMdi::fileModified()
{
    qDebug()<<"MyMdi::fileModified()";
    setWindowModified(true);
    IsFileSaved = false;

    if(CurrFilePath.isEmpty())
    {
        setWindowTitle("*"+CurrFileName);
    }
    else
    {
        setWindowTitle("*"+CurrFilePath);
    }
}
void MyMdi::closeEvent(QCloseEvent *event)
{
    qDebug()<<"MyMdi::closeEvent()";
    if(IsFileSaved)
    {
        event->accept();
    }
    else
    {
        QMessageBox::StandardButton  ret = QMessageBox::warning(this,
                             tr("Save File"),
                             tr("File %1 has been changed, Do you want to save the file?").arg(CurrFileName),
                             QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        if(ret == QMessageBox::Save)
        {
            SaveAs();
            event->accept();
        }
        else if(ret == QMessageBox::Discard)
        {
            event->accept();
        }
        else if(ret == QMessageBox::Cancel)
        {
            event->ignore();
        }
        else
        {
            event->ignore();
        }
    }
}
QString MyMdi::GetCurrFileName()
{
    qDebug()<<"MyMdi::GetCurrFileName()";
    return CurrFileName;
}
void MyMdi::SetCurrFileName(QString fileName)
{
    qDebug()<<"MyMdi::SetCurrFileName()";
    CurrFileName = fileName;
}
void MyMdi::Print()
{
    qDebug()<<"MyMdi::Print()";
    QPrinter printer;
    if(QPrintDialog(&printer, this).exec() == QPrintDialog::Accepted)
    {
        print(&printer);
    }
}
bool MyMdi::GetSaveStatus()
{
    qDebug()<<"MyMdi::GetSaveStatus()";
    return IsFileSaved;
}
bool MyMdi::GetIsUntitled()
{
    qDebug()<<"MyMdi::GetIsUntitled()";
    return IsUntitled;
}
bool MyMdi::RenameFile()
{
    qDebug()<<"MyMdi::RenameFile()";
    QString newFileName = QFileDialog::getSaveFileName(this,
                                                       tr("Save As"),
                                                       CurrFilePath,
                                                       tr("Text Document(*.txt)"));
    if(!newFileName.isEmpty())
    {
        QFile del(CurrFileName);
        if(!del.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Error"), del.errorString());
            return false;
        }
        del.rename(newFileName);
        setWindowTitle(newFileName);
        CurrFileName = QFileInfo(newFileName).fileName();
        CurrFilePath = QFileInfo(newFileName).filePath();
        IsUntitled = false;
        del.close();
        return true;
    }
    else
    {
        return false;
    }
}
void MyMdi::setTextColor()
{
    qDebug()<<"MyMdi::setTextColor()start";
    unSetTextColor();
    if(textCursor().hasSelection())
    {
        searchString = textCursor().selectedText();
        if(!searchString.contains(QString(" ")))
        {
            setStyleSheet("QPlainTextEdit{selection-background-color: #FFA500; selection-color: #000000;}");
            Find(searchString, QTextDocument::FindWholeWords, true);
        }
        else
        {
            setStyleSheet("QPlainTextEdit{selection-background-color: #EEDDDD; selection-color: #000000;}");
        }
    }
    else
    {
        highlightCurrentLine();
    }
    qDebug()<<"MyMdi::setTextColor()end";
}

void MyMdi::unSetTextColor()
{
    qDebug()<<"MyMdi::unSetTextColor()start";
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection extraSelection;
    extraSelection.cursor = this->textCursor();
    extraSelection.format = textCursor().charFormat();
    extraSelection.format.setBackground(Qt::white);
    extraSelection.format.setForeground(Qt::black);
    extraSelection.format.setProperty(QTextFormat::FullWidthSelection,true);
    extraSelections.append(extraSelection);
    setExtraSelections(extraSelections);
    qDebug()<<"MyMdi::unSetTextColor()end";
}
QList<QString> MyMdi::Find(QString searchString, QTextDocument::FindFlags options, bool ifMarkNeed)
{
    qDebug()<<"MyMdi::Find()";
    qDebug()<<"searchString = "<<searchString;
    QList<QString> lineNoHolder;
    this->searchString = searchString;

    QString lineContent;
    if(ifMarkNeed)
    {
        QTextCursor cursor(textDocument);
        QTextCharFormat colorFormat(cursor.charFormat());
        colorFormat.setBackground(Qt::green);
        textcursor = textCursor();
        textcursor.movePosition(QTextCursor::Start);
        textcursor = textDocument->find(searchString, textcursor, options);

        QList<QTextEdit::ExtraSelection> extraSelections = this->extraSelections();
        if(!textcursor.isNull())
        {
            QTextEdit::ExtraSelection extraSelection;
            extraSelection.cursor = textcursor;
            extraSelection.format = colorFormat;
            extraSelections.append(extraSelection);

            while (!textcursor.isNull() && !textcursor.atEnd())
            {
                textcursor = textDocument->find(searchString, textcursor, options);
                if (!textcursor.isNull())
                {
                    extraSelection.cursor = textcursor;
                    //extraSelection.cursor.clearSelection();
                    extraSelection.format = colorFormat;
                    extraSelections.append(extraSelection);
                }
            }
        }
        setExtraSelections(extraSelections);
        return lineNoHolder;
    }
    else
    {
        textcursor = textCursor();
        textcursor.movePosition(QTextCursor::Start);
        while (!textcursor.isNull() && !textcursor.atEnd())
        {
            textcursor = textDocument->find(searchString, textcursor, options);
            if (!textcursor.isNull())
            {
                int lineNo = textcursor.blockNumber() + 1;
                lineContent = tr("    Line %1: %2").arg(lineNo).arg(textcursor.block().text());
                lineNoHolder.append(lineContent);
            }
        }
        return lineNoHolder;
    }
    /*QTextEdit::ExtraSelection extraSelection_block;
    textcursor = textCursor();
    textcursor.movePosition(QTextCursor::StartOfLine);
    textcursor = textDocument->find(searchString, textcursor, options);
    lineNoHolder.append(textcursor.blockNumber());
    extraSelection_block.cursor = textcursor;
    extraSelection_block.cursor.clearSelection();
    extraSelection_block.format.setBackground(QColor(200,255,255,255));
    extraSelection_block.format.setForeground(Qt::black);
    extraSelection_block.format.setProperty(QTextFormat::FullWidthSelection,true);

    QList<QTextEdit::ExtraSelection> extraSelections = this->extraSelections();
    extraSelections.append(extraSelection_block);
    while (!cursor.isNull() && !cursor.atEnd())
    {
        QTextEdit::ExtraSelection extraSelection;
        cursor = textDocument->find(searchString, cursor, options);
        lineNoHolder.append(cursor.blockNumber());
        if (!cursor.isNull())
        {
            extraSelection.cursor = cursor;
            extraSelection.format = colorFormat;
            extraSelections.append(extraSelection);
        }
    }
    setExtraSelections(extraSelections);
    return lineNoHolder;*/

}
void MyMdi::FindNext(QString searchString,
                     QTextDocument::FindFlags options,
                     bool searchLoop,
                     bool searchDirection)
{
    qDebug()<<"MyMdi::FindNext()";
    unSetTextColor();
    QTextCursor cursor(textDocument);
    QTextCharFormat colorFormat(cursor.charFormat());
    colorFormat.setBackground(QColor("#FF4040"));
    isFirstSearch = true;

    QTextEdit::ExtraSelection extraSelection_block;
    textcursor = textCursor();
    textcursor = textDocument->find(searchString, textcursor, options);
    qDebug()<<textcursor.selectedText();
    if(!textcursor.isNull())
    {
        qDebug()<<1;
        extraSelection_block.cursor = textcursor;
        //extraSelection_block.cursor.clearSelection();
        extraSelection_block.format = colorFormat;
        //in case of there are two search result in same line.
        if(searchDirection)
        {
            qDebug()<<2;
            textcursor.movePosition(QTextCursor::WordLeft, QTextCursor::MoveAnchor,0);
        }
        else
        {
            qDebug()<<3;
            textcursor.movePosition(QTextCursor::WordRight, QTextCursor::MoveAnchor,0);
        }
        qDebug()<<4;
        //textcursor.clearSelection();
        setTextCursor(textcursor);
        qDebug()<<5;
    }
    else if(searchLoop)
    {
        if(searchDirection)
        {
            qDebug()<<6;
            textcursor = textCursor();
            textcursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
            textcursor = textDocument->find(searchString, textcursor, options);
            extraSelection_block.cursor = textcursor;
            //extraSelection_block.cursor.clearSelection();
            extraSelection_block.format = colorFormat;
            //in case of there are two search result in same line.
            textcursor.movePosition(QTextCursor::WordLeft, QTextCursor::MoveAnchor,0);
            setTextCursor(textcursor);
        }
        else
        {
            qDebug()<<7;
            textcursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
            textcursor = textDocument->find(searchString, textcursor, options);
            extraSelection_block.cursor = textcursor;
            //extraSelection_block.cursor.clearSelection();
            extraSelection_block.format = colorFormat;
            //in case of there are two search result in same line.
            textcursor.movePosition(QTextCursor::WordRight, QTextCursor::MoveAnchor,0);
            setTextCursor(textcursor);
        }
    }
    else
    {
        QMessageBox message;
        message.setWindowTitle("Find");
        message.setText(tr("Can't find the text \n \"%1\"").arg(searchString));
        message.setIcon(QMessageBox::NoIcon);
        message.setWindowModality(Qt::WindowModal);
        message.setWindowFlags(Qt::Drawer);
        message.exec();
        //QMessageBox::information(this, "Find",tr("Can't find the text \n \"%1\"").arg(searchString));
    }
    qDebug()<<8;
    Find(searchString, options, true);
    QList<QTextEdit::ExtraSelection> extraSelections = this->extraSelections();
    removeDuplicateExtraSelection(extraSelection_block);
    extraSelections.append(extraSelection_block);
    setExtraSelections(extraSelections);
    /*
    if(isFirstSearch)
    {
        qDebug()<<1;
        textcursor = textDocument->find(searchString, textcursor, options);
        qDebug()<<textcursor.selectedText();
        if(!textcursor.isNull())
        {
            qDebug()<<2;
            extraSelection_block.cursor = textcursor;
            extraSelection_block.cursor.clearSelection();
            extraSelection_block.format.setBackground(QColor(200,255,255,255));
            extraSelection_block.format.setForeground(Qt::black);
            extraSelection_block.format.setProperty(QTextFormat::FullWidthSelection,true);
            isFirstSearch = false;
            //in case of there are two search result in same line.
            textcursor.movePosition(QTextCursor::WordRight, QTextCursor::MoveAnchor);
            textcursor.clearSelection();
            setTextCursor(textcursor);
        }
    }
    qDebug()<<3;
    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(extraSelection_block);
    while (!textcursor.isNull() && !textcursor.atEnd())
    {
        qDebug()<<4;
        QTextEdit::ExtraSelection extraSelection;
        textcursor = textDocument->find(searchString, textcursor, options);
        qDebug()<<cursor.selectedText();
        if (!textcursor.isNull())
        {
            qDebug()<<5;
            extraSelection.cursor = textcursor;
            extraSelection.format = colorFormat;
            extraSelections.append(extraSelection);
            totalCount ++;
        }
        else if(searchLoop)
        {
            if(searchDirection)
            {
                qDebug()<<6;
                textcursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
            }
            else
            {
                qDebug()<<7;
                textcursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
            }
        }
    }
    qDebug()<<8;
    setExtraSelections(extraSelections);*/
}
int MyMdi::GetTotalCount(QTextDocument::FindFlags options)
{
    qDebug()<<"MyMdi::GetTotalCount()";
    totalCount = 0;
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::Start);
    qDebug()<<"searchString = "<<searchString;
    while (!cursor.isNull() && !cursor.atEnd())
    {
        cursor = this->document()->find(searchString, cursor, options);
        if (!cursor.isNull())
        {
            totalCount++;
            qDebug()<<"totalCount = "<<totalCount;
        }
    }

    QMessageBox message;
    message.setWindowTitle("Count");
    message.setText(tr("%1 match(es) to occurrence(s)").arg(totalCount));
    message.setParent(this);
    message.setWindowFlags(Qt::Drawer);
    message.exec();

    return totalCount;
}
void MyMdi::MarkLines(QList<QPoint> lineHolder)
{
    qDebug()<<"MyMdi::MarkLines()";
    QPainter painter(this);
    painter.setBrush(Qt::blue);
    painter.setRenderHint(QPainter::Antialiasing);
    for( int i = 0; i< lineHolder.size(); i++)
    {
        painter.drawEllipse(lineHolder.at(i).x(), lineHolder.at(i).y(), fontMetrics().height()/3, fontMetrics().height()/3);
    }
}
void MyMdi::highlightSearchString()
{
    qDebug()<<"MyMdi::highlightSearchString()";
    if(textCursor().hasSelection())
    {
        QList<QTextEdit::ExtraSelection> extraSelections = this->extraSelections();
        QTextCursor cursor(textDocument);
        QTextCharFormat colorFormat(cursor.charFormat());
        colorFormat.setBackground(Qt::red);
        QTextEdit::ExtraSelection extraSelection;
        extraSelection.cursor = textCursor();
        //extraSelection.cursor.clearSelection();
        extraSelection.format = colorFormat;
        removeDuplicateExtraSelection(extraSelection);
        extraSelections.append(extraSelection);
        setExtraSelections(extraSelections);
    }
}
void MyMdi::clearMark()
{
    qDebug()<<"MyMdi::clearMark()";
    setTextColor();
    unSetTextColor();
}

QList<QString> MyMdi::searchCurrentFile(QString searchString)
{
    qDebug()<<"MyMdi::searchCurrentFile()";
    return Find(searchString, QTextDocument::FindWholeWords, true);
}

void MyMdi::resetCursorPosition()
{
    qDebug()<<"MyMdi::resetCursorPosition()";
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::Start);
    this->setTextCursor(cursor);
}

void MyMdi::removeDuplicateExtraSelection(QTextEdit::ExtraSelection extraSelection)
{
    /*QList<QTextEdit::ExtraSelection> extraSelections =  this->extraSelections();
    for(int i = 0; i < extraSelections.size();i++)
    {
        if(extraSelections.at(i).cursor.position() == extraSelection.cursor.position())
        {
            extraSelections.removeAt(i);
            qDebug()<<"MyMdi::removeDuplicateExtraSelection() remove success";
        }
        break;
    }*/
}

QString MyMdi::getFilePath()
{
    return CurrFilePath;
}

QTextBlock MyMdi::getFirstVisibleBlock()
{
    return this->firstVisibleBlock();
}
