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
#include "QSignalMapper"
#include "mydockwidget.h"
#include "QMdiArea"
const static QString RENCENT_FILE_KEY = "Recent File History";
MainWindow::MainWindow(QWidget *parent) : mdiArea(new QMdiArea)
{
    qDebug()<<"MainWindow()";
    mdiArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea.setViewMode(QMdiArea::TabbedView);
    mdiArea.setTabShape(QTabWidget::Rounded);
    mdiArea.setTabsClosable(false);
    mdiArea.setTabsMovable(true);
    mdiArea.setDocumentMode(true);
    mdiArea.setStyleSheet("QTabBar{max-width:280px}");

    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(OpenRecentFile(QString)));

    setCentralWidget(&mdiArea);
    //ReadSetting();
    CreateMenus();
    createStatusBar();
    setWindowTitle(tr("Muti-Text Document Editor"));
    connect(this, &MainWindow::mdiWindowCntChanged, this, &MainWindow::createDefaultMdiWindow);
    connect(this, &MainWindow::mdiWindowCntChanged, this, &MainWindow::setTabBarWidth);
    connect(&mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(setWindowsTitle(QMdiSubWindow*)));
    SetmdiWindowCnt(0);
    this->showMaximized();
}

void MainWindow::setTabBarWidth()
{
    qDebug()<<"MainWindow::setTabBarWidth";
    int totalMdiWindowCnt = mdiArea.subWindowList().size();
    QString style = tr("QTabBar{max-width:%1px}").arg(250*totalMdiWindowCnt);
    qDebug()<<"style = "<<style;
    mdiArea.setStyleSheet(style);
}

void MainWindow::setWindowsTitle(QMdiSubWindow *subWindow)
{
    qDebug()<<"MainWindow::setWindowsTitle start";
    if(subWindow)
    {
        if(qobject_cast<MyMdi *>(subWindow->widget())->getFilePath().isEmpty())
        {
            setWindowTitle(qobject_cast<MyMdi *>(subWindow->widget())->GetCurrFileName());
        }
        else
        {
            setWindowTitle(qobject_cast<MyMdi *>(subWindow->widget())->getFilePath());
        }
    }
    qDebug()<<"MainWindow::setWindowsTitle end";
}

void MainWindow::createStatusBar()
{
    qDebug()<<"createStatusBar()";
    statusBar()->showMessage(tr("Ready"));
}
void MainWindow::SetmdiWindowCnt(int windowCount)
{
    qDebug()<<"SetmdiWindowCnt()";
    totalMdiWindowCnt = windowCount;
    emit mdiWindowCntChanged();
}

int MainWindow::GetmdiWindowCnt()
{
    qDebug()<<"GetmdiWindowCnt()";
    return mdiArea.subWindowList().size();
}

void MainWindow::setSearchString()
{
    qDebug()<<"setSearchString()";
    GetActiveMdiWindow()->setSearchStringFromTab(dialog->findtab->comboBox->currentText());
    searchString = dialog->findtab->comboBox->currentText();
}

void MainWindow::findAllFiles(bool MatchWholeWord, bool MatchUpperLower)
{
    qDebug()<<"MainWindow:findAllFiles()start";
    QList<QMdiSubWindow *> myMdiList = mdiArea.subWindowList();
    QList<QString> searchResult;
    searchString = dialog->findtab->comboBox->currentText();
    qDebug()<<"searchString = "<<searchString;
    dockWidgetSearchString = searchString;
    int totalFound = 0;
    int totalFileFound = 0;
    for(int i = 0; i < myMdiList.size(); i++)
    {
        MyMdi *mymdi = qobject_cast<MyMdi *> (myMdiList.at(i)->widget());
        QList<QString> tempResult;
        if(MatchWholeWord && MatchUpperLower)
        {
            tempResult = mymdi->Find(searchString,QTextDocument::FindWholeWords|QTextDocument::FindCaseSensitively, false);
        }
        else if (MatchUpperLower)
        {
            tempResult = mymdi->Find(searchString,QTextDocument::FindCaseSensitively, false);
        }
        else
        {
            tempResult = mymdi->Find(searchString,QTextDocument::FindWholeWords, false);
        }
        if(tempResult.size()>0)
        {
            totalFound += tempResult.size();
            tempResult.insert(0,QString("  ") + mymdi->getFilePath() + QString(tr("(%1 hits)").arg(tempResult.size())));
            searchResult += tempResult;
            totalFileFound++;
        }
    }
    if(searchResult.size()>0)
    {
        searchResult.insert(0,QString(tr("Search \"%1\" (%2 hits in %3 files)").arg(searchString).arg(totalFound)).arg(totalFileFound));
        if(IsDockWidgetInitialized)
        {
            myDockWidget->close();
        }

        myDockWidget = new MyDockWidget(tr("Find result(%1 hits)").arg(totalFound),this);
        myDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
        this->addDockWidget(Qt::BottomDockWidgetArea,myDockWidget);
        myDockWidget->setResult(searchResult);
        myDockWidget->updateTextArea(searchResult);
        myDockWidget->show();
        IsDockWidgetInitialized = true;
        connect(myDockWidget->textEdit, SIGNAL(notifyLineLocate(QString, int)), this, SLOT(gotoLine(QString, int)));
        connect(myDockWidget, &MyDockWidget::destroyed, this, &MainWindow::setDockWidgetStatus);
        dialog->close();
    }
    qDebug()<<"MainWindow:findAllFiles()end";
}

void MainWindow::createDefaultMdiWindow()
{
    qDebug()<<"MainWindow::createDefaultMdiWindow() start";
    if(mdiArea.subWindowList().size() == 0)
    {
        MyMdi *child = createSubWindow();
        child->NewFile();
        child->show();
        child->showMaximized();
        this->setWindowTitle(child->GetCurrFileName());
        child->IsDefaultWindow = true;
        mdiArea.setFocus();
        //child->resetCursorPosition();
        //connect(child, &MyMdi::destroyed, this, &MainWindow::mdiWindowCntChanged);
    }
    qDebug()<<"MainWindow::createDefaultMdiWindow() end";
}

MyMdi* MainWindow::createSubWindow()
{
    qDebug()<<"createSubWindow()";
    MyMdi *child = new MyMdi;
    mdiArea.addSubWindow(child);
    emit mdiWindowCntChanged();
    return child;
}
void MainWindow::NewFile()
{
    qDebug()<<"MainWindow::NewFile()";
    MyMdi *child = createSubWindow();
    child->NewFile();
    child->show();
    child->showMaximized();
    this->setWindowTitle(child->GetCurrFileName());
}

void MainWindow::OpenFile()
{
    qDebug()<<"MainWindow::OpenFile()";
    QString fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        QMdiSubWindow *existSubWindow = (QMdiSubWindow *)FindChildSubWindow(fileName);
        if(existSubWindow)
        {
            mdiArea.setActiveSubWindow(existSubWindow);
            //this->setWindowTitle(qobject_cast<MyMdi *>(existSubWindow->widget())->getFilePath());
        }
        else
        {
            MyMdi *child = createSubWindow();
            if(child->LoadFile(fileName))
            {
                child->show();
                //this->setWindowTitle(child->getFilePath());
                UpdateHistory(fileName);
                if(mdiArea.subWindowList().size() > 1)
                {
                    QList<QMdiSubWindow *> mdiList = mdiArea.subWindowList();
                    for(int i = 0; i<mdiList.size(); i++)
                    {
                        MyMdi *mymdi = qobject_cast<MyMdi *>(mdiList.at(i)->widget());
                        if(mymdi)
                        {
                            if(mymdi->IsUntitled &&mymdi->IsFileSaved &&mymdi->IsDefaultWindow)
                            {
                                mdiArea.setActiveSubWindow(mdiList.at(i));
                                mdiArea.closeActiveSubWindow();
                                break;
                            }
                        }
                    }
                }
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
                //this->setWindowTitle(qobject_cast<MyMdi *>(existSubWindow->widget())->getFilePath());
            }
            else
            {
                MyMdi *child = createSubWindow();
                if(child->LoadFile(fileName))
                {
                    child->show();
                    child->setWindowTitle(fileName);
                    //this->setWindowTitle(child->getFilePath());
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
            //this->setWindowTitle(qobject_cast<MyMdi *>(existSubWindow->widget())->getFilePath());
        }
        else
        {
            MyMdi *child = createSubWindow();
            if(child->LoadFile(fileName))
            {
                child->show();
                child->setWindowTitle(fileName);
                //this->setWindowTitle(child->getFilePath());
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
            //this->setWindowTitle(mymdi->getFilePath());
        }
        else if(ret == QMessageBox::Discard)
        {
            mymdi->LoadFile(mymdi->CurrFilePath);
            this->setWindowTitle(mymdi->getFilePath());
            UpdateHistory(mymdi->CurrFileName);
        }
    }
    else
    {
        mymdi->LoadFile(mymdi->CurrFilePath);
        this->setWindowTitle(mymdi->getFilePath());
        UpdateHistory(mymdi->CurrFileName);
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
        {
            statusBar()->showMessage(tr("File Saved"), 2000);
            this->setWindowTitle(GetActiveMdiWindow()->getFilePath());
        }
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
        this->setWindowTitle(GetActiveMdiWindow()->getFilePath());
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
        this->setWindowTitle(GetActiveMdiWindow()->getFilePath());
        UpdateHistory(GetActiveMdiWindow()->CurrFileName);
    }
}

void MainWindow::RenameFile()
{
    qDebug()<<"MainWindow::RenameFile()";
    GetActiveMdiWindow()->RenameFile();
    this->setWindowTitle(GetActiveMdiWindow()->getFilePath());
    UpdateHistory(GetActiveMdiWindow()->CurrFileName);
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
    qDebug()<<"MainWindow::CloseOthers()";
    QMdiSubWindow *subwindow = mdiArea.activeSubWindow();
    QList<QMdiSubWindow *> subWindowList = mdiArea.subWindowList();
    for(int i = 0; i< subWindowList.size(); i++)
    {
        if(subwindow != subWindowList.at(i))
        {
            mdiArea.setActiveSubWindow(subWindowList.at(i));
            mdiArea.closeActiveSubWindow();
        }
    }
}

void MainWindow::DeleteFromDisk()
{
    qDebug()<<"MainWindow::DeleteFromDisk()";
    QString filename = GetActiveMdiWindow()->CurrFilePath;
    qDebug()<<"filename = "<<filename;
    QFile in(filename);
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
    qDebug()<<"MainWindow::Print()";
    GetActiveMdiWindow()->Print();
}

void MainWindow::PrintNow()
{
    qDebug()<<"MainWindow::PrintNow()";
    GetActiveMdiWindow()->Print();
}

void MainWindow::OpenRecentFile(QString fileName)
{
    qDebug()<<"MainWindow::OpenRecentFile()";
    Openfile(fileName);
}

void MainWindow::OpenAllRecentFile()
{
    qDebug()<<"MainWindow::OpenAllRecentFile()";
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
    qDebug()<<"MainWindow::ClearRecentHistory()";
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.remove(RENCENT_FILE_KEY);
    RefreshFileMenu();
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
    qDebug()<<"MainWindow::RefreshFileMenu()";
    if(GetActiveMdiWindow()!= 0)
    {
        if(GetActiveMdiWindow()->IsUntitled)
        {
            ActionCloseFile->setEnabled(false);
            ActionSave->setEnabled(true);
            ActionSaveAll->setEnabled(true);
            ActionRename->setEnabled(true);
            ActionDeleteFile->setEnabled(false);
            ActionSaveAs->setEnabled(true);
            ActionReload->setEnabled(false);
        }
        else
        {
            qDebug()<<2;
            if(GetActiveMdiWindow()->document()->isModified())
            {
                ActionSave->setEnabled(true);
                ActionSaveAll->setEnabled(true);
            }
            else
            {
                ActionSave->setEnabled(false);
                ActionSaveAll->setEnabled(false);
            }

            ActionRename->setEnabled(true);
            ActionDeleteFile->setEnabled(true);
            ActionReload->setEnabled(true);
            ActionCloseFile->setEnabled(true);
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
        ActionReload->setEnabled(false);
        ActionCloseFile->setEnabled(false);
    }

    if(mdiArea.subWindowList().size()>1)
    {
        ActionCloseOtherFiles->setEnabled(true);
        ActionCloseAllFile->setEnabled(true);
        ActionCloseFile->setEnabled(true);
    }
    else
    {
        ActionCloseOtherFiles->setEnabled(false);
        ActionCloseAllFile->setEnabled(false);
    }
}

MyMdi *MainWindow::GetActiveMdiWindow()
{
    qDebug()<<"MainWindow::GetActiveMdiWindow()start";
    QMdiSubWindow *subWindow = mdiArea.activeSubWindow();
    if(subWindow)
    {
        qDebug()<<0;
        return qobject_cast<MyMdi *>(subWindow->widget());
    }
    else
    {
        mdiArea.setActiveSubWindow(mdiArea.subWindowList().at(0));
        mdiArea.show();
        subWindow = mdiArea.activeSubWindow();

        if(subWindow)
        {
            qDebug()<<1111;
            return qobject_cast<MyMdi *>(subWindow->widget());
        }
        else
        {
            qDebug()<<2222;
            return 0;
        }
    }
    qDebug()<<"MainWindow::GetActiveMdiWindow()end";
}

QStringList MainWindow::ReadHistory()
{
    qDebug()<<"MainWindow::ReadHistory()";
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
    qDebug()<<"MainWindow::UpdateHistory()";
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
    qDebug()<<"MainWindow::CreateMenus()";
    /*--------File Menu Start -------*/
    QMenu *FileMenu = menuBar()->addMenu(tr("&File"));
    connect(FileMenu, &QMenu::aboutToShow, this, &MainWindow::RefreshFileMenu);
    QToolBar *toolBar = addToolBar(tr("File"));

    ActionNew = FileMenu->addAction(tr("New"));
    ActionNew->setShortcut(QKeySequence::New);
    connect(ActionNew, &QAction::triggered, this, &MainWindow::NewFile);
    ActionNew->setStatusTip("Create New File");
    toolBar->addAction(ActionNew);

    ActionOpen = FileMenu->addAction(tr("Open"),this, &MainWindow::OpenFile, QKeySequence::Open);
    ActionOpen->setStatusTip(tr("Open file from computer"));
    toolBar->addAction(ActionOpen);

    ActionReload = FileMenu->addAction(tr("Reload File"));
    connect(ActionReload, &QAction::triggered, this, &MainWindow::ReloadFile);
    ActionReload->setToolTip(tr("ReOpen file"));

    ActionSave = FileMenu->addAction(tr("Save"),this, &MainWindow::Save,  QKeySequence::Save);
    ActionSave->setStatusTip(tr("save the file"));
    toolBar->addAction(ActionSave);

    ActionSaveAs = FileMenu->addAction(tr("Save As"));
    ActionSaveAs->setShortcut(QKeySequence::SaveAs);
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
    ActionCloseFile->setShortcut(QKeySequence::Close);
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
        QString filePath = HistoryFileList.at(i);
        historyAction = FileMenu->addAction(QString::number(i+1) + ":" + filePath);
        mapper->setMapping(historyAction, filePath);
        connect(historyAction, SIGNAL(triggered()), mapper, SLOT(map()));
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
    qDebug()<<"MainWindow::SelectAll()";
    GetActiveMdiWindow()->selectAll();
}

void MainWindow::RefreshEditMenu()
{
    qDebug()<<"MainWindow::RefreshEditMenu()";
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
    qDebug()<<"MainWindow::mouseDoubleClickEvent()";
    QMainWindow::mouseDoubleClickEvent(event);

}

void MainWindow::Find()
{
    qDebug()<<"MainWindow::Find() start";
    QTextCursor cursor;
    if(GetActiveMdiWindow())
    {
        qDebug()<<"a";
        cursor = GetActiveMdiWindow()->textCursor();
        if(cursor.hasSelection())
        {
            dialog = new TabDialog(cursor.selectedText(),tabFind, this);
            dialog->installEventFilter(this);
            dialog->setFixedSize(480,320);
            dialog->setWindowFlags(Qt::Drawer);
            connect(dialog->findtab, SIGNAL(notifySearchNextClicked(QString, bool, bool, bool, bool)),
                                   this, SLOT(FindNextFromTabDialog(QString, bool, bool, bool, bool)));
            connect(dialog->findtab, SIGNAL(notifyCountClicked(bool,bool)), this, SLOT(TotalCount(bool, bool)));
            connect(dialog->findtab->comboBox, &QComboBox::editTextChanged, this, &MainWindow::setSearchString);
            connect(dialog->findtab, SIGNAL(notifySearchAllOpenedFileClicked(bool, bool)), this, SLOT(findAllFiles(bool, bool)));
            dialog->show();
        }
        else
        {
            dialog = new TabDialog(QString(""),tabFind, this);
            dialog->installEventFilter(this);
            dialog->setFixedSize(480,320);
            dialog->setWindowFlags(Qt::Drawer);
            connect(dialog->findtab, SIGNAL(notifySearchNextClicked(QString, bool, bool, bool, bool)),
                                   this, SLOT(FindNextFromTabDialog(QString, bool, bool, bool, bool)));
            connect(dialog->findtab, SIGNAL(notifyCountClicked(bool,bool)), this, SLOT(TotalCount(bool, bool)));
            connect(dialog->findtab->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setSearchString);
            connect(dialog->findtab, SIGNAL(notifySearchAllOpenedFileClicked(bool, bool)), this, SLOT(findAllFiles(bool, bool)));
            dialog->show();
        }
    }
    else
    {
        qDebug()<<"b";
        dialog = new TabDialog(QString(""),tabFind, this);
        dialog->installEventFilter(this);
        dialog->setFixedSize(480,320);
        dialog->setWindowFlags(Qt::Drawer);
        connect(dialog->findtab, SIGNAL(notifySearchNextClicked(QString, bool, bool, bool, bool)),
                               this, SLOT(FindNextFromTabDialog(QString, bool, bool, bool, bool)));
        connect(dialog->findtab, SIGNAL(notifyCountClicked(bool,bool)), this, SLOT(TotalCount(bool, bool)));
        connect(dialog->findtab->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setSearchString);
        connect(dialog->findtab, SIGNAL(notifySearchAllOpenedFileClicked(bool, bool)), this, SLOT(findAllFiles(bool, bool)));
        dialog->show();
    }
    qDebug()<<"MainWindow::Find() end";
}

void MainWindow::FindInFile()
{
    qDebug()<<"MainWindow::FindInFile()";

    QTextCursor cursor = GetActiveMdiWindow()->textCursor();
    if(cursor.hasSelection())
    {
        dialog = new TabDialog(cursor.selectedText(),tabFile, this);
        dialog->installEventFilter(this);
        dialog->setFixedSize(480,320);
        dialog->setWindowFlags(Qt::Drawer);
        connect(dialog->documentTab, SIGNAL(notifySearchNextClicked(QString, bool, bool, bool, bool)),
                               this, SLOT(FindNextFromTabDialog(QString, bool, bool, bool, bool)));

        connect(dialog->findtab, SIGNAL(notifyCountClicked(bool,bool)), this, SLOT(TotalCount(bool, bool)));
        connect(dialog->findtab->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setSearchString);
        dialog->show();
    }
    else
    {
        dialog = new TabDialog(QString(""),tabFile, this);
        dialog->installEventFilter(this);
        dialog->setFixedSize(480,320);
        dialog->setWindowFlags(Qt::Drawer);
        connect(dialog->documentTab, SIGNAL(notifySearchNextClicked(QString, bool, bool, bool, bool)),
                               this, SLOT(FindNextFromTabDialog(QString, bool, bool, bool, bool)));

        connect(dialog->findtab, SIGNAL(notifyCountClicked(bool,bool)), this, SLOT(TotalCount(bool, bool)));
        connect(dialog->findtab->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setSearchString);
        dialog->show();
    }
}

void MainWindow::FindAndReplace()
{
    qDebug()<<"MainWindow::FindAndReplace()";
    QTextCursor cursor = GetActiveMdiWindow()->textCursor();
    if(cursor.hasSelection())
    {
        dialog = new TabDialog(cursor.selectedText(),tabReplace, this);
        dialog->installEventFilter(this);
        dialog->setFixedSize(480,320);
        dialog->setWindowFlags(Qt::Drawer);
        connect(dialog->replaceTab, SIGNAL(notifySearchNextClicked(QString, bool, bool, bool, bool)),
                               this, SLOT(FindNextFromTabDialog(QString, bool, bool, bool, bool)));
        connect(dialog->findtab, SIGNAL(notifyCountClicked(bool,bool)), this, SLOT(TotalCount(bool, bool)));
        connect(dialog->findtab->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setSearchString);
        dialog->show();
    }
    else
    {
        dialog = new TabDialog(QString(""),tabReplace, this);
        dialog->installEventFilter(this);
        dialog->setFixedSize(480,320);
        dialog->setWindowFlags(Qt::Drawer);
        connect(dialog->replaceTab, SIGNAL(notifySearchNextClicked(QString, bool, bool, bool, bool)),
                               this, SLOT(FindNextFromTabDialog(QString, bool, bool, bool, bool)));
        connect(dialog->findtab, SIGNAL(notifyCountClicked(bool,bool)), this, SLOT(TotalCount(bool, bool)));
        connect(dialog->findtab->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::setSearchString);
        dialog->show();
    }
}

void MainWindow::FindNextFromTabDialog(QString searchString, bool matchWholeWord, bool matchUpperLower, bool SearchLoop, bool searchDirection)
{
    qDebug()<<"MainWindow::FindNextFromTabDialog()";
    if(matchWholeWord && matchUpperLower && searchDirection)
    {
        qDebug()<<"matchWholeWord && matchUpperLower && searchDirection";
        GetActiveMdiWindow()->FindNext(searchString,
                                       QTextDocument::FindWholeWords|QTextDocument::FindCaseSensitively|QTextDocument::FindBackward,
                                       SearchLoop,
                                       searchDirection);
    }
    else if(matchWholeWord && matchUpperLower)
    {
        qDebug()<<"matchWholeWord && matchUpperLower";
        GetActiveMdiWindow()->FindNext(searchString,
                                       QTextDocument::FindWholeWords|QTextDocument::FindCaseSensitively,
                                       SearchLoop,
                                       searchDirection);
    }
    else if(matchWholeWord && searchDirection)
    {
        qDebug()<<"matchWholeWord && searchDirection";
        GetActiveMdiWindow()->FindNext(searchString,
                                       QTextDocument::FindWholeWords|QTextDocument::FindBackward,
                                       SearchLoop,
                                       searchDirection);
    }
    else if(matchUpperLower && searchDirection)
    {
        qDebug()<<"matchUpperLower && searchDirection";
        GetActiveMdiWindow()->FindNext(searchString,
                                       QTextDocument::FindCaseSensitively|QTextDocument::FindBackward,
                                       SearchLoop,
                                       searchDirection);
    }
    else if(matchUpperLower)
    {
        qDebug()<<"matchUpperLower";
        GetActiveMdiWindow()->FindNext(searchString,
                                       QTextDocument::FindCaseSensitively,
                                       SearchLoop,
                                       searchDirection);
    }
    else if(matchWholeWord)
    {
        qDebug()<<"matchWholeWord";
        GetActiveMdiWindow()->FindNext(searchString,
                                       QTextDocument::FindWholeWords,
                                       SearchLoop,
                                       searchDirection);
    }
    else if(searchDirection)
    {
        qDebug()<<"searchDirection";
        GetActiveMdiWindow()->FindNext(searchString,
                                       QTextDocument::FindBackward,
                                       SearchLoop,
                                       searchDirection);
    }
    else
    {
        qDebug()<<"Just search";
        GetActiveMdiWindow()->FindNext(searchString,
                                       QTextDocument::FindWholeWords,
                                       SearchLoop,
                                       searchDirection);
    }
}

void MainWindow::FindNext()
{
    qDebug()<<"MainWindow::FindNext()";
    qDebug()<<"searchString = "<<searchString;
    if(GetActiveMdiWindow()->textCursor().hasSelection())
    {
        searchString = GetActiveMdiWindow()->textCursor().selectedText();
        GetActiveMdiWindow()->FindNext(searchString, QTextDocument::FindWholeWords, true, false);
    }
    else if(!searchString.isEmpty())
    {
        GetActiveMdiWindow()->FindNext(searchString, QTextDocument::FindWholeWords, true, false);
    }
}

void MainWindow::MainWindow::FindPrev()
{
    qDebug()<<"MainWindow::FindPrev()";
    if(GetActiveMdiWindow()->textCursor().hasSelection())
    {
        searchString = GetActiveMdiWindow()->textCursor().selectedText();
        GetActiveMdiWindow()->FindNext(searchString, QTextDocument::FindBackward, true, true);
    }
    else if(!searchString.isEmpty())
    {
        GetActiveMdiWindow()->FindNext(searchString, QTextDocument::FindBackward, true, true);
    }
}

void MainWindow::ColumnLocate()
{
    qDebug()<<"MainWindow::ColumnLocate()";
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

void MainWindow::TotalCount(bool matchWholeWord, bool matchCaseSencitive)
{
    if(matchWholeWord && matchCaseSencitive)
    {
        GetActiveMdiWindow()->GetTotalCount(QTextDocument::FindWholeWords|QTextDocument::FindCaseSensitively);
    }
    else if(matchCaseSencitive)
    {
        GetActiveMdiWindow()->GetTotalCount(QTextDocument::FindCaseSensitively);
    }
    else
    {
        GetActiveMdiWindow()->GetTotalCount(QTextDocument::FindWholeWords);
    }
}

void MainWindow::gotoLine(QString filePath, int lineNo)
{
    qDebug()<<"MainWindow::gotoLine()";
    QList<QMdiSubWindow *> myMdiList = mdiArea.subWindowList();
    for(int i = 0; i < myMdiList.size(); i++)
    {
        MyMdi *myMdi = qobject_cast<MyMdi *> (myMdiList.at(i)->widget());
        qDebug()<<"filepath = "<<myMdi->getFilePath();
        if(myMdi->getFilePath() == filePath)
        {
            qDebug()<<"1";
            mdiArea.setActiveSubWindow(myMdiList.at(i));
            mdiArea.activeSubWindow()->showMaximized();
            QTextCursor cursor = myMdi->textCursor();
            cursor.setPosition(0);
            cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor,lineNo-1);
            myMdi->setTextCursor(cursor);
            QTextBlock block = cursor.block();
            if(block.blockNumber()<=16)
            {
                cursor = myMdi->document()->find(dockWidgetSearchString,cursor);
                myMdi->setTextCursor(cursor);
            }
            else if(block == GetActiveMdiWindow()->getFirstVisibleBlock())
            {
                cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor,16);
                myMdi->setTextCursor(cursor);
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor,16);
                cursor = myMdi->document()->find(dockWidgetSearchString,cursor);
                myMdi->setTextCursor(cursor);
            }
            else
            {
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor,16);
                myMdi->setTextCursor(cursor);
                cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor,16);
                cursor = myMdi->document()->find(dockWidgetSearchString,cursor);
                myMdi->setTextCursor(cursor);
            }
        }
    }
}

void MainWindow::setDockWidgetStatus()
{
    IsDockWidgetInitialized = false;
}

void MainWindow::SaveCopyText()
{
    qDebug()<<"MainWindow::SaveCopyText()";
    GetActiveMdiWindow()->CopySaveAs();
}

MyMdi *MainWindow::FindChildSubWindow(QString filename)
{
    qDebug()<<"MainWindow::FindChildSubWindow()";
    return mdiArea.findChild<MyMdi *>(filename);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<"MainWindow::closeEvent()";
    mdiArea.closeAllSubWindows();
    QMainWindow::closeEvent(event);
}
