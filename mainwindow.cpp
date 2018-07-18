#include "mainwindow.h"
#include "mymdi.h"
#include "QMenu"
#include "QMenuBar"
#include "QMessageBox"
#include "QAction"
#include "QFileDialog"
#include "QStatusBar"
#include "QSettings"
#include "QCoreApplication"
#include "QToolBar"
#include "QTabBar"
#include "QClipboard"
#include "QLabel"
#include "tabdialog.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QRadioButton"
#include "QLineEdit"
#include "QPushButton"
#include "QTextBlock"
#include "QPlainTextEdit"
#include "QDebug"
const static QString RENCENT_FILE_KEY = "Recent File History";
MainWindow::MainWindow(QWidget *parent) : mdiArea(new QMdiArea)
{
    qDebug()<<"MainWindow()";
    mdiArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(&mdiArea);
    //ReadSetting();
    CreateMenus();
    createStatusBar();
    setWindowTitle(tr("Muti-Text Document Editor"));
    connect(this, &MainWindow::mdiWindowCntChanged, this, &MainWindow::createDefaultMdiWindow);
    SetmdiWindowCnt(0);
}

void MainWindow::createStatusBar()
{
    qDebug()<<"createStatusBar()";
    statusBar()->showMessage(tr("Ready"));
}
void MainWindow::SetmdiWindowCnt(int windowCount)
{
    totalMdiWindowCnt = windowCount;
    emit mdiWindowCntChanged();
}

int MainWindow::GetmdiWindowCnt()
{
    return totalMdiWindowCnt;
}

void MainWindow::createDefaultMdiWindow()
{
    qDebug()<<"MainWindow::createDefaultMdiWindow()";
    if(mdiArea.subWindowList().size() == 0)
    {
        MyMdi *child = createSubWindow();
        child->NewFile();
        child->show();
        connect(child, &MyMdi::destroyed, this, &MainWindow::mdiWindowCntChanged);
    }
}

MyMdi* MainWindow::createSubWindow()
{
    qDebug()<<"createSubWindow()";
    MyMdi *child = new MyMdi;
    mdiArea.addSubWindow(child);
    qDebug()<<1.2;
    //connect(child, &MyMdi::copyAvailable, this, &MainWindow::Copy);
    //connect(child, &MyMdi::copyAvailable, this, &MainWindow::Cut);
    connect(child, &MyMdi::selectionChanged, this, &MainWindow::setTextColor);
    connect(child, &MyMdi::cursorPositionChanged, this, &MainWindow::unSetTextColor);
    SetmdiWindowCnt(GetmdiWindowCnt()+1);
    return child;
}
void MainWindow::NewFile()
{
    qDebug()<<"MainWindow::NewFile()";
    qDebug()<<1;
    MyMdi *child = createSubWindow();
    qDebug()<<2;
    child->NewFile();
    qDebug()<<3;
    child->show();
    qDebug()<<4;
}

void MainWindow::OpenFile()
{
    qDebug()<<"MainWindow::OpenFile()";
    qDebug()<<1;
    QString fileName = QFileDialog::getOpenFileName(this);
    qDebug()<<2;
    if(!fileName.isEmpty())
    {
        QMdiSubWindow *existSubWindow = (QMdiSubWindow *)FindChildSubWindow(fileName);
        if(existSubWindow)
        {
            mdiArea.setActiveSubWindow(existSubWindow);
        }
        else
        {
            MyMdi *child = createSubWindow();
            if(child->LoadFile(fileName))
            {
                child->show();
                child->setWindowTitle(fileName);
                UpdateHistory(fileName);
            }
            else
            {
                child->close();
            }
        }
    }
}

void MainWindow::Openfile(QString fileName)
{
    qDebug()<<"MainWindow::Openfile()";
    if(fileName.isEmpty() &&fileName != QString("0"))
    {
        fileName = QFileDialog::getOpenFileName(this);
        if(!fileName.isEmpty())
        {
            QMdiSubWindow *existSubWindow = (QMdiSubWindow *)FindChildSubWindow(fileName);
            if(existSubWindow)
            {
                mdiArea.setActiveSubWindow(existSubWindow);
            }
            else
            {
                MyMdi *child = createSubWindow();
                if(child->LoadFile(fileName))
                {
                    child->show();
                    child->setWindowTitle(fileName);
                    UpdateHistory(fileName);
                }
                else
                {
                    child->close();
                }
            }
        }
    }
    else
    {
        QMdiSubWindow *existSubWindow = (QMdiSubWindow *)FindChildSubWindow(fileName);
        if(existSubWindow)
        {
            mdiArea.setActiveSubWindow(existSubWindow);
        }
        else
        {
            MyMdi *child = createSubWindow();
            if(child->LoadFile(fileName))
            {
                child->show();
                child->setWindowTitle(fileName);
                UpdateHistory(fileName);
            }
            else
            {
                child->close();
                emit mdiWindowCntChanged();
            }
        }
    }
}

void MainWindow::ReloadFile()
{
    qDebug()<<"MainWindow::ReloadFile()";
    MyMdi *mymdi = GetActiveMdiWindow();
    if(mymdi->GetIsUntitled()||mymdi->GetSaveStatus())
    {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Warning"),
                             tr("File has been changed, do you want to save the file?"),
                             QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        if(ret == QMessageBox::Save)
        {
            mymdi->SaveAs();
        }
        else if(ret == QMessageBox::Discard)
        {
            mymdi->LoadFile(mymdi->GetCurrFileName());
        }
    }
    else
    {
        mymdi->LoadFile(mymdi->GetCurrFileName());
    }
}

void MainWindow::SaveAsCopy()
{
    qDebug()<<"MainWindow::SaveAsCopy()";
    GetActiveMdiWindow()->CopySaveAs();
}

void MainWindow::Save()
{
    qDebug()<<"MainWindow::Save()";
    if(GetActiveMdiWindow())
    {
        if(GetActiveMdiWindow()->Save())
            statusBar()->showMessage(tr("File Saved"), 2000);
        if(GetActiveMdiWindow())
        {
            UpdateHistory(GetActiveMdiWindow()->GetCurrFileName());
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("Save File Error"));
    }
}

void MainWindow::SaveAs()
{
    qDebug()<<"MainWindow::SaveAs()";
    if(GetActiveMdiWindow()->SaveAs()&&GetActiveMdiWindow())
    {
        statusBar()->showMessage(tr("File Saved"), 2000);
    }
    UpdateHistory(GetActiveMdiWindow()->GetCurrFileName());
}

void MainWindow::SaveAll()
{
    qDebug()<<"MainWindow::SaveAll()";
    QList<QMdiSubWindow *> subWindowList = mdiArea.subWindowList();
    for(int i = 0; i< subWindowList.size(); i++)
    {
        subWindowList[i]->activateWindow();
        GetActiveMdiWindow()->Save();
    }
}

void MainWindow::RenameFile()
{
    qDebug()<<"MainWindow::RenameFile()";
    GetActiveMdiWindow()->RenameFile();
}

void MainWindow::Close()
{
    qDebug()<<"MainWindow::Close()";
    mdiArea.closeActiveSubWindow();
    emit mdiWindowCntChanged();
}

void MainWindow::CloseAll()
{
    qDebug()<<"MainWindow::CloseAll()";
    mdiArea.closeAllSubWindows();
    emit mdiWindowCntChanged();
}

void MainWindow::CloseOthers()
{
    qDebug()<<"CloseOthers()";
    QMdiSubWindow *subwindow = (QMdiSubWindow *)GetActiveMdiWindow();
    mdiArea.closeAllSubWindows();
    mdiArea.addSubWindow(subwindow);
    mdiArea.show();
}

void MainWindow::DeleteFromDisk()
{
    qDebug()<<"DeleteFromDisk()";
    QFile in(GetActiveMdiWindow()->GetCurrFileName());
    if(in.remove())
    {
        mdiArea.closeActiveSubWindow();
        emit mdiWindowCntChanged();
        setStatusTip(tr("File Delete!"));
    }
    else
    {
        QMessageBox::warning(this,
                             tr("File Delete Error"),
                             tr("Fail to Delete File :%1").arg(in.errorString()));
    }
}

void MainWindow::Print()
{
    qDebug()<<"Print()";
    GetActiveMdiWindow()->Print();
}

void MainWindow::PrintNow()
{
    qDebug()<<"PrintNow()";
    GetActiveMdiWindow()->Print();
}

void MainWindow::OpenRecentFile(QString fileName)
{
    qDebug()<<"OpenRecentFile()";
    Openfile(fileName);
}

void MainWindow::OpenAllRecentFile()
{
    qDebug()<<"OpenAllRecentFile()";
    QStringList fileList = ReadHistory();
    if(!fileList.isEmpty())
    {
        for(int i = 0; i<fileList.size(); i++)
        {
            Openfile(fileList.at(i));
        }
    }
}

void MainWindow::ClearRecentHistory()
{
    qDebug()<<"ClearRecentHistory()";
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.remove(RENCENT_FILE_KEY);
}

void MainWindow::Exit()
{
    qDebug()<<"Exit()";
    for(int i = 0; i < mdiArea.subWindowList().size(); i++)
    {
        mdiArea.activeSubWindow();
        QString fileName = GetActiveMdiWindow()->GetCurrFileName();
        UpdateHistory(fileName);
        GetActiveMdiWindow()->close();
        emit mdiWindowCntChanged();
    }
}

void MainWindow::RefreshFileMenu()
{
    qDebug()<<"RefreshFileMenu()";
    if(GetActiveMdiWindow()!= 0)
    {
        qDebug()<<"inside";
        if(GetActiveMdiWindow()->IsUntitled)
        {
            QTextDocument *document = GetActiveMdiWindow()->document();

            if(!document->isModified())
            {
                ActionSave->setEnabled(false);
                ActionSaveAll->setEnabled(false);
                ActionRename->setEnabled(false);
                ActionDeleteFile->setEnabled(false);
            }
            else
            {
                ActionSave->setEnabled(true);
                ActionSaveAll->setEnabled(true);
            }
            ActionSaveAs->setEnabled(true);
        }
        else
        {
            ActionSave->setEnabled(true);
            ActionSaveAll->setEnabled(true);
            ActionRename->setEnabled(true);
            ActionDeleteFile->setEnabled(true);
        }
        ActionSaveAs->setEnabled(true);
    }
    else
    {
        ActionSave->setEnabled(false);
        ActionSaveAll->setEnabled(false);
        ActionSaveAs->setEnabled(false);
        ActionRename->setEnabled(false);
        ActionDeleteFile->setEnabled(false);
    }
}

MyMdi *MainWindow::GetActiveMdiWindow()
{
    qDebug()<<"GetActiveMdiWindow()";
    if(QMdiSubWindow *subWindow = mdiArea.activeSubWindow())
    {
        return qobject_cast<MyMdi *>(subWindow->widget());
    }
    else
    {
        if(mdiArea.subWindowList().size()>0)
        {
            mdiArea.activatePreviousSubWindow();
            QMdiSubWindow *subWindow = mdiArea.activeSubWindow();
            return qobject_cast<MyMdi *>(subWindow->widget());
        }
        else
        {
            MyMdi *child = createSubWindow();
            child->NewFile();
            child->show();
            return child;
        }
    }
}

QStringList MainWindow::ReadHistory()
{
    qDebug()<<"ReadHistory()";
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QStringList fileList;
    int count = settings.beginReadArray(RENCENT_FILE_KEY);
    for(int i = 0; i < count; i++)
    {
        settings.setArrayIndex(i);
        fileList.append(settings.value(RENCENT_FILE_KEY).toString());
    }
    settings.endArray();
    return fileList;
}

void MainWindow::UpdateHistory(QString fileName)
{
    qDebug()<<"UpdateHistory()";
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QStringList oldFileList = ReadHistory();
    if(!oldFileList.contains(fileName))
    {
        oldFileList.append(fileName);
    }
    else
    {
        oldFileList.removeAll(fileName);
        oldFileList.append(fileName);
    }
    settings.beginWriteArray(RENCENT_FILE_KEY);
    for (int i = 0; i<oldFileList.size(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue(RENCENT_FILE_KEY, oldFileList.at(i));
    }
    settings.endArray();
}

void MainWindow::CreateMenus()
{
    qDebug()<<"CreateMenus()";
    /*--------File Menu Start -------*/
    QMenu *FileMenu = menuBar()->addMenu(tr("&File"));
    connect(FileMenu, &QMenu::aboutToShow, this, &MainWindow::RefreshFileMenu);
    QToolBar *toolBar = addToolBar(tr("File"));

    ActionNew = FileMenu->addAction(tr("New"));
    ActionNew->setShortcut(QKeySequence("ctrl + N"));
    connect(ActionNew, &QAction::triggered, this, &MainWindow::NewFile);
    ActionNew->setStatusTip("Create New File");
    toolBar->addAction(ActionNew);

    ActionOpen = FileMenu->addAction(tr("Open"),this, &MainWindow::OpenFile, QKeySequence("ctrl + O"));
    ActionOpen->setStatusTip(tr("Open file from computer"));
    toolBar->addAction(ActionOpen);

    ActionReload = FileMenu->addAction(tr("Reload File"));
    connect(ActionReload, &QAction::triggered, this, &MainWindow::ReloadFile);
    ActionReload->setToolTip(tr("ReOpen file"));

    ActionSave = FileMenu->addAction(tr("Save"),this, &MainWindow::Save,  QKeySequence("ctrl + S"));
    ActionSave->setStatusTip(tr("save the file"));
    toolBar->addAction(ActionSave);

    ActionSaveAs = FileMenu->addAction(tr("Save As"));
    connect(ActionSaveAs, &QAction::triggered, this, &MainWindow::SaveAs);
    ActionSaveAs->setStatusTip(tr("Save file to another name or distination"));

    ActionSaveCopy = FileMenu->addAction(tr("Save Copy Text As"));
    connect(ActionSaveCopy, &QAction::triggered, this, &MainWindow::SaveCopyText);
    ActionSaveCopy->setStatusTip(tr("Save copy text to another file"));

    ActionSaveAll = FileMenu->addAction(tr("save All the file"),this, &MainWindow::SaveAll, QKeySequence("ctrl + shift + s"));
    ActionSaveAll->setStatusTip(tr("save all the file"));

    ActionRename = FileMenu->addAction(tr("Rename the file"));
    connect(ActionRename, &QAction::triggered, this, &MainWindow::RenameFile);
    ActionRename->setStatusTip(tr("Rename the file"));

    ActionCloseFile = FileMenu->addAction(tr("close"));
    connect(ActionCloseFile, &QAction::triggered, this, &MainWindow::Close);
    ActionCloseFile->setStatusTip(tr("Close the file"));

    ActionCloseAllFile = FileMenu->addAction(tr("close All"));
    connect(ActionCloseAllFile, &QAction::triggered, this, &MainWindow::CloseAll);
    ActionCloseAllFile->setStatusTip(tr("Close All the files"));

    ActionCloseOtherFiles = FileMenu->addAction(tr("close others"));
    connect(ActionCloseOtherFiles, &QAction::triggered, this, &MainWindow::CloseOthers);
    ActionCloseOtherFiles->setStatusTip(tr("Close All the files except current one"));

    ActionDeleteFile = FileMenu->addAction(tr("Delete File"));
    connect(ActionDeleteFile, &QAction::triggered, this, &MainWindow::DeleteFromDisk);
    ActionDeleteFile->setStatusTip(tr("Delete File from Disk"));

    FileMenu->addSeparator();

    ActionPrint = FileMenu->addAction(tr("Print"));
    connect(ActionPrint, &QAction::triggered, this, &MainWindow::Print);
    ActionPrint->setStatusTip(tr("Print the file"));

    ActionPrintNow = FileMenu->addAction(tr("Print Now"));
    connect(ActionPrintNow, &QAction::triggered, this, &MainWindow::PrintNow);
    ActionPrintNow->setStatusTip(tr("Print the file Now"));

    FileMenu->addSeparator();
    
    QStringList HistoryFileList = ReadHistory();
    for( int i = 0; i < HistoryFileList.size(); i++)
    {
        FileMenu->addAction(QString(i) + ":" + HistoryFileList.at(i),
                            this, SLOT(OpenRecentFile(QString)));
    }
    FileMenu->addSeparator();
    
    FileMenu->addAction("Open All Recent File", this, &MainWindow::OpenAllRecentFile);
    FileMenu->addAction("Clear recent file list", this, &MainWindow::ClearRecentHistory);
    
    FileMenu->addSeparator();
    
    ActionExit = FileMenu->addAction(tr("Exit"),this, &MainWindow::Close, QKeySequence("Alt + F4"));
    ActionExit->setStatusTip(tr("Close the Application"));
    /*--------File Menu End -------*/
    
    
    /*--------Edit Menu Start -------*/
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    connect(editMenu, &QMenu::aboutToShow, this, &MainWindow::RefreshEditMenu);
    ActionUndo = editMenu->addAction(tr("UnDo"), this, &MainWindow::Undo, QKeySequence("ctrl + z"));
    ActionRedo = editMenu->addAction(tr("Redo"), this, &MainWindow::Redo, QKeySequence("ctrl + y"));
    editMenu->addSeparator();

    ActionCut = editMenu->addAction(tr("Cut"), this, &MainWindow::Cut, QKeySequence("ctrl + x"));
    ActionCopy = editMenu->addAction(tr("Copy"), this, &MainWindow::Copy, QKeySequence("ctrl + c"));
    ActionPaste = editMenu->addAction(tr("Paste"), this, &MainWindow::Paste, QKeySequence("ctrl + v"));
    ActionSelectAll = editMenu->addAction(tr("Select All"), this, &MainWindow::SelectAll, QKeySequence("ctrl + a"));
    editMenu->addSeparator();

    editMenu->addMenu(tr("Copy to Clipbroad"));
    editMenu->addMenu(tr("Shrink"));
    editMenu->addMenu(tr("View Transfer"));
    editMenu->addMenu(tr("Execute Column"));
    editMenu->addMenu(tr("Comment"));
    editMenu->addMenu(tr("Auto Complete"));
    editMenu->addAction(tr("Clean White Space"));
    editMenu->addSeparator();

    editMenu->addAction(tr("Column Edit Mode"));
    editMenu->addSeparator();

    editMenu->addAction(tr("Set as Read Only"));
    editMenu->addAction(tr("Clear Read Only"));
    /*--------Edit Menu End -------*/

    /*--------Search Menu Start -------*/
    QMenu *searchMenu = menuBar()->addMenu(tr("&Search"));
    searchMenu->addAction(tr("Search"),this, &MainWindow::Find, QKeySequence("ctrl + F"));
    searchMenu->addAction(tr("Search in the file"),this, &MainWindow::FindInFile, QKeySequence("ctrl + Shift + F"));
    searchMenu->addAction(tr("Find Next"),this, &MainWindow::FindNext, QKeySequence("F3"));
    searchMenu->addAction(tr("Find Previous"),this, &MainWindow::FindPrev, QKeySequence("Shift + F3"));
    searchMenu->addAction(tr("Quick Find Next"),this, &MainWindow::FindNext, QKeySequence("ctrl + F3"));
    searchMenu->addAction(tr("Quick Find Previous"),this, &MainWindow::FindPrev, QKeySequence("ctrl + Shift + F3"));
    searchMenu->addAction(tr("Replace"),this, &MainWindow::FindAndReplace, QKeySequence("ctrl + H"));
    searchMenu->addAction(tr("Special Research"),this, &MainWindow::Find, QKeySequence("ctrl + Alt + I"));
    searchMenu->addAction(tr("Column Locate"),this, &MainWindow::ColumnLocate, QKeySequence("ctrl + G"));
    //searchMenu->addAction(tr("Locate pair brace"),this, &MainWindow::LocateBrace, QKeySequence("ctrl + B"));
    editMenu->addSeparator();

    editMenu->addMenu(tr("Mark All"));
    editMenu->addMenu(tr("Cancel Mark"));
    editMenu->addSeparator();

    searchMenu->addAction(tr("Set/Cancel Note"));
    searchMenu->addAction(tr("Next Note"));
    searchMenu->addAction(tr("Previous Note"));
    searchMenu->addAction(tr("Clear All Note"));
    searchMenu->addAction(tr("Cut Note"));
    searchMenu->addAction(tr("Copy Note"));
    searchMenu->addAction(tr("Paste(Replace) Note"));
    searchMenu->addAction(tr("Delete Note"));

}

void MainWindow::Undo()
{
    qDebug()<<"MainWindow::Undo()";
    GetActiveMdiWindow()->textDocument->undo();
}

void MainWindow::Redo()
{
    qDebug()<<"MainWindow::Redo()";
    GetActiveMdiWindow()->textDocument->redo();
}

void MainWindow::Copy()
{
    qDebug()<<"MainWindow::Copy()";
    GetActiveMdiWindow()->copy();
}

void MainWindow::Paste()
{
    qDebug()<<"MainWindow::Paste()";
    GetActiveMdiWindow()->paste();
}

void MainWindow::Cut()
{
    qDebug()<<"MainWindow::Cut()";
    GetActiveMdiWindow()->cut();
}

void MainWindow::SelectAll()
{
    qDebug()<<"SelectAll()";
    GetActiveMdiWindow()->selectAll();
}

void MainWindow::RefreshEditMenu()
{
    qDebug()<<"RefreshEditMenu()";
    if(GetActiveMdiWindow())
    {
        MyMdi *subWindow = GetActiveMdiWindow();
        if(subWindow->textDocument->isRedoAvailable())
            ActionRedo->setEnabled(true);
        else
            ActionRedo->setEnabled(false);

        if(subWindow->textDocument->isUndoAvailable())
           ActionUndo->setEnabled(true);
        else
            ActionUndo->setEnabled(false);

        if(subWindow&&subWindow->textCursor().hasSelection())
        {
            ActionCopy->setEnabled(true);
            ActionCut->setEnabled(true);
        }
        else
        {
            ActionCopy->setEnabled(false);
            ActionCut->setEnabled(false);
        }

        if(subWindow->canPaste())
        {
            ActionPaste->setEnabled(true);
        }
        else
        {
            ActionPaste->setEnabled(false);
        }
    }
    else
    {
        ActionCopy->setEnabled(false);
        ActionCut->setEnabled(false);
        ActionPaste->setEnabled(false);
        ActionUndo->setEnabled(false);
        ActionRedo->setEnabled(false);
        ActionSelectAll->setEnabled(false);
    }
}
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug()<<"mouseDoubleClickEvent()";
    QMainWindow::mouseDoubleClickEvent(event);

}

void MainWindow::Find()
{
    qDebug()<<"MainWindow::Find()";
    TabDialog *dialog = new TabDialog("abc",tabFind, this);
    dialog->setFixedSize(480,320);
    dialog->setWindowIcon(QIcon(QPixmap(0,0)));
    dialog->show();
}

void MainWindow::FindInFile()
{
    qDebug()<<"FindInFile()";
    TabDialog *dialog = new TabDialog("abc",tabFile, this);
    dialog->setFixedSize(480,320);
    dialog->setWindowIcon(QIcon(QPixmap(0,0)));
    dialog->show();
}

void MainWindow::FindAndReplace()
{
    qDebug()<<"FindAndReplace()";
    TabDialog *dialog = new TabDialog("abc",tabReplace, this);
    dialog->setFixedSize(480,320);
    dialog->setWindowIcon(QIcon(QPixmap(0,0)));
    dialog->show();
}

void MainWindow::FindNext()
{
    qDebug()<<"FindNext()";
    GetActiveMdiWindow()->FindNext(searchString, QTextDocument::FindWholeWords);
}

void MainWindow::FindPrev()
{
    qDebug()<<"createStatusBar()";
    GetActiveMdiWindow()->FindNext(searchString, QTextDocument::FindBackward);
}

void MainWindow::ColumnLocate()
{
    qDebug()<<"ColumnLocate()";
    QDialog *dialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout;

    QHBoxLayout *levelOnelayout = new QHBoxLayout;
    QRadioButton *button = new QRadioButton(tr("Column"));
    button->setFixedWidth(130);
    QRadioButton *buttonOffSet = new QRadioButton(tr("Off Set"));
    buttonOffSet->setFixedWidth(130);
    button->setChecked(true);
    levelOnelayout->addWidget(button);
    levelOnelayout->addWidget(buttonOffSet);
    levelOnelayout->setContentsMargins(0,15,5,15);
    levelOnelayout->setAlignment(Qt::AlignLeft);

    QHBoxLayout *levelTwolayout = new QHBoxLayout;
    QLabel *currentPosition =  new QLabel(tr("Current Position:"));
    currentPosition->setFixedWidth(130);
    currentPosition->setAlignment(Qt::AlignRight);
    currentPosition->setContentsMargins(0,0,40,0);
    QLabel *currentPositionValue = new QLabel(this);
    currentPositionValue->setText(tr("300"));
    currentPositionValue->setFixedWidth(120);
    levelTwolayout->addWidget(currentPosition);
    levelTwolayout->addWidget(currentPositionValue);
    levelTwolayout->setAlignment(Qt::AlignLeft);

    QHBoxLayout *levelThreelayout = new QHBoxLayout;
    QLabel *targetPosition  = new QLabel(tr("Target Position:"));
    targetPosition->setFixedWidth(130);
    targetPosition->setAlignment(Qt::AlignRight|Qt::AlignCenter);
    targetPosition->setContentsMargins(0,0,40,0);
    QLineEdit *targetPositionValue = new QLineEdit(this);
    targetPositionValue->setFixedWidth(120);
    targetPositionValue->setText(tr("200"));
    QPushButton *buttonLocate = new QPushButton(tr("Locate"));
    buttonLocate->setFixedWidth(110);
    levelThreelayout->addWidget(targetPosition);
    levelThreelayout->addWidget(targetPositionValue);
    levelThreelayout->addWidget(buttonLocate);
    levelThreelayout->setAlignment(Qt::AlignLeft);

    QHBoxLayout *levelFourlayout = new QHBoxLayout;
    QLabel *stopPosition  = new QLabel(tr("Stop Position:"));
    stopPosition->setFixedWidth(130);
    stopPosition->setAlignment(Qt::AlignRight|Qt::AlignCenter);
    stopPosition->setContentsMargins(0,0,40,0);
    QLabel *stopPositionValue = new QLabel(this);
    stopPositionValue->setFixedWidth(120);
    stopPositionValue->setText(tr("150"));
    QPushButton *buttonCancel = new QPushButton(tr("Cancel"));
    buttonCancel->setFixedWidth(110);
    levelFourlayout->addWidget(stopPosition);
    levelFourlayout->addWidget(stopPositionValue);
    levelFourlayout->addWidget(buttonCancel);
    levelFourlayout->setAlignment(Qt::AlignLeft);

    layout->addLayout(levelOnelayout);
    layout->addLayout(levelTwolayout);
    layout->addLayout(levelThreelayout);
    layout->addLayout(levelFourlayout);
    dialog->setLayout(layout);
    dialog->setFixedSize(400,150);
    dialog->setWindowTitle(tr("Locate Column No"));
    dialog->show();

    QTextCursor cursor = GetActiveMdiWindow()->textCursor();
    int currentBlockNo = cursor.blockNumber();
    int lastBlockNo = GetActiveMdiWindow()->textDocument->blockCount();
    currentPositionValue->setText(QString(currentBlockNo));
    stopPositionValue->setText(QString(lastBlockNo));
}

void MainWindow::setTextColor()
{
    qDebug()<<"MainWindow::setTextColor()";
    if(GetActiveMdiWindow())
        GetActiveMdiWindow()->setTextColor();
    else
        qDebug()<<"Error";
}

void MainWindow::unSetTextColor()
{
    qDebug()<<"MainWindow::unSetTextColor()";
    if(GetActiveMdiWindow())
        GetActiveMdiWindow()->unSetTextColor();
    else
        qDebug()<<"Error";
}

void MainWindow::gotoLine(int lineNo)
{
    qDebug()<<"gotoLine()";
    QTextCursor cursor = GetActiveMdiWindow()->textCursor();
    cursor.setPosition(0);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor,lineNo);
    GetActiveMdiWindow()->setTextCursor(cursor);
    unSetTextColor();
}

void MainWindow::SaveCopyText()
{
    qDebug()<<"SaveCopyText()";
    GetActiveMdiWindow()->CopySaveAs();
}

MyMdi *MainWindow::FindChildSubWindow(QString filename)
{
    qDebug()<<"FindChildSubWindow()";
    return mdiArea.findChild<MyMdi *>(filename);
}
