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

MainWindow::MainWindow(QWidget *parent) : mdiArea(new QMdiArea)
{
    mdiArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(&mdiArea);
    ReadSetting();
    CreateMenus();
    createStatusBar();
    setWindowTitle(tr("Muti-Text Document Editor"));

}
MyMdi MainWindow::createSubWindow()
{
    MyMdi *child = new MyMdi();
    mdiArea.addSubWindow(child);

    connect(child, &QTextEdit::copyAvailable, this, &MainWindow::Copy);
    connect(child, &QTextEdit::copyAvailable, this, &MainWindow::Cut);
    return child;
}
void MainWindow::NewFile()
{
    MyMdi *child = createSubWindow();
    child->NewFile();
    child->show();
}

void MainWindow::OpenFile(QString fileName = null)
{
    if(fileName.isEmpty())
    {
        fileName = QFileDialog::getOpenFileName(this);
        if(!fileName.isEmpty())
        {
            QMdiSubWindow *existSubWindow = FindChildSubWindow(fileName);
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
        QMdiSubWindow *existSubWindow = FindChildSubWindow(fileName);
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

void MainWindow::ReloadFile()
{
    MyMdi *mymdi = GetActiveMdiWindow();
    if(mymdi->GetIsUntitled()||mymdi->GetSaveStatus())
    {
        QMessageBox::standardButton ret = QMessageBox::warning(tr("Warning"),
                             tr("File has been changed, do you want to save the file?"),
                             QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        if(ret = QMessageBox::Save)
        {
            mymdi->SaveAs();
        }
        else if(ret = QMessageBox::Discard)
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

    GetActiveMdiWindow().CopySaveAs();
}

void MainWindow::Save()
{
    if(activeMdiWindow().Save()&&activeMdiWindow())
        statusBar()->showMessage(tr("File Saved"), 2000);
    UpdateHistory(activeMdiWindow().GetCurrFileName());
}

void MainWindow::SaveAs()
{
    if(activeMdiWindow().SaveAs()&&activeMdiWindow())
    {
        statusBar()->showMessage(tr("File Saved"), 2000);
    }
    UpdateHistory(activeMdiWindow().GetCurrFileName());
}

void MainWindow::SaveAll()
{
    QList<QMdiSubWindow *> subWindowList = mdiArea.subWindowList();
    for(int i = 0; i< subWindowList.size(); i++)
    {
        subWindowList[i]->activateWindow();
        GetActiveMdiWindow().Save();
    }
}

void MainWindow::RenameFile()
{
    GetActiveMdiWindow().RenameFile();
}

void MainWindow::Close()
{
    mdiArea.closeActiveSubWindow();
}

void MainWindow::CloseAll()
{
    mdiArea.closeAllSubWindows();
}

void MainWindow::CloseOthers()
{
    QMdiSubWindow subwindow = GetActiveMdiWindow();
    mdiArea.closeAllSubWindows();
    mdiArea.addSubWindow(subwindow);
    mdiArea.show();
}

void MainWindow::DeleteFromDisk()
{
    QFile in(GetActiveMdiWindow().GetCurrFileName());
    if(in.remove())
    {
        mdiArea.closeActiveSubWindow();
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
    GetActiveMdiWindow().Print();
}

void MainWindow::PrintNow()
{
    GetActiveMdiWindow().Print();
}

void MainWindow::OpenRecentFile(QString fileName)
{
    OpenFile(fileName);
}

void MainWindow::OpenAllRecentFile()
{
    QStringList fileList = ReadHistory();
    if(!fileList.isEmpty())
    {
        for(int i = 0; i<fileList.size(); i++)
        {
            OpenFile(fileList.at(i));
        }
    }
}

void MainWindow::ClearRecentHistory()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.remove(RENCENT_FILE_KEY);
}

void MainWindow::Exit()
{
    for(int i = 0; i<mdiArea.size(); i++)
    {
        mdiArea.activeSubWindow();
        QString fileName = GetActiveMdiWindow().GetCurrFileName();
        UpdateHistory(fileName);
        GetActiveMdiWindow().close();
    }
}

void MainWindow::RefreshFileMenu()
{
    if(GetActiveMdiWindow().IsUntitled)
    {
        if(!GetActiveMdiWindow().document()->isModified())
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
    }
    else
    {
        ActionSave->setEnabled(true);
        ActionSaveAll->setEnabled(true);
        ActionRename->setEnabled(true);
        ActionDeleteFile->setEnabled(true);
    }



}


MyMdi MainWindow::GetActiveMdiWindow()
{
    if(QMdiSubWindow *subWindow = mdiArea.activeSubWindow())
        return qobject_cast<MyMdi *>(subWindow->widget());
    return 0;
}

const static QString RECENT_FILE_KEY = "Recent File History";
QStringList MainWindow::ReadHistory()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QStringList fileList;
    int count = settings.beginReadArray(RECENT_FILE_KEY);
    for(int i = 0; i < count; i++)
    {
        settings.setArrayIndex(i);
        fileList.append(settings.value(RECENT_FILE_KEY).toString());
    }
    settings.endArray();
    return fileList;
}

void MainWindow::UpdateHistory(QString fileName)
{
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
    settings.beginWriteArray(RECENT_FILE_KEY);
    for (int i = 0; i<oldFileList.size(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue(RECENT_FILE_KEY, oldFileList.at(i));
    }
    settings.endArray();
}

void MainWindow::CreateMenus()
{
    /*--------File Menu Start -------*/
    QMenu *FileMenu = menuBar()->addMenu(tr("&File"));
    connect(FileMenu, &QMenu::triggered, this, &MainWindow::RefreshFileMenu);
    QToolBar *toolBar = addToolBar(tr("File"));

    QAction *ActionNew = FileMenu->addAction(tr("New"), QKeySequence("ctrl + N"));
    connect(ActionNew, &QAction::triggered, this, &MainWindow::NewFile);
    ActionNew->setStatusTip("Create New File");
    toolBar->addAction(ActionNew);

    QAction *ActionOpen = FileMenu->addAction(tr("Open"),QKeySequence("ctrl + O"));
    connect(ActionOpen, &QAction::triggered, this, &MainWindow::OpenFile);
    ActionOpen->setStatusTip(tr("Open file from computer"));
    toolBar->addAction(ActionOpen);

    QAction *ActionReload = FileMenu->addAction(tr("Reload File"));
    connect(ActionReload, &QAction::triggered, this, &MainWindow::ReloadFile);
    ActionReload->setToolTip(tr("ReOpen file"));

    QAction *ActionSave = FileMenu->addAction(tr("Save"), QKeySequence("ctrl + S"));
    connect(ActionSave, &QAction::triggered, this, &MainWindow::Save);
    ActionSave->setStatusTip(tr("save the file"));
    toolBar->addAction(ActionSave);

    QAction *ActionSaveAs = FileMenu->addAction(tr("Save As"));
    connect(ActionSaveAs, &QAction::triggered, this, &MainWindow::SaveAs);
    ActionSaveAs->setStatusTip(tr("Save file to another name or distination"));

    QAction *ActionSaveCopy = FileMenu->addAction(tr("Save Copy Text As"));
    connect(ActionSaveCopy, &QAction::triggered, this, &MainWindow::SaveCopyText);
    ActionSaveCopy->setStatusTip(tr("Save copy text to another file"));

    QAction *ActionSaveAll = FileMenu->addAction(tr("save All the file"), QKeySequence("ctrl + shift + s"));
    connect(ActionSaveAll, &QAction::triggered, this, &MainWindow::SaveAll);
    ActionSaveAll->setStatusTip(tr("save all the file"));

    QAction *ActionRename = FileMenu->addAction(tr("Rename the file"));
    connect(ActionRename, &QAction::triggered, this, &MainWindow::RenameFile);
    ActionRename->setStatusTip(tr("Rename the file"));

    QAction *ActionCloseFile = FileMenu->addAction(tr("close"));
    connect(ActionCloseFile, &QAction::triggered, this, &MainWindow::Close);
    ActionCloseFile->setStatusTip(tr("Close the file"));

    QAction *ActionCloseAllFile = FileMenu->addAction(tr("close All"));
    connect(ActionCloseAllFile, &QAction::triggered, this, &MainWindow::CloseAll);
    ActionCloseAllFile->setStatusTip(tr("Close All the files"));

    QAction *ActionCloseOtherFiles = FileMenu->addAction(tr("close others"));
    connect(ActionCloseOtherFiles, &QAction::triggered, this, &MainWindow::CloseOthers);
    ActionCloseOtherFiles->setStatusTip(tr("Close All the files except current one"));

    QAction *ActionDeleteFile = FileMenu->addAction(tr("Delete File"));
    connect(ActionDeleteFile, &QAction::triggered, this, &MainWindow::DeleteFromDisk);
    ActionDeleteFile->setStatusTip(tr("Delete File from Disk"));

    FileMenu->addSeparator();

    QAction *ActionPrint = FileMenu->addAction(tr("Print"));
    connect(ActionPrint, &QAction::triggered, this, &MainWindow::Print);
    ActionPrint->setStatusTip(tr("Print the file"));

    QAction *ActionPrintNow = FileMenu->addAction(tr("Print Now"));
    connect(ActionPrintNow, &QAction::triggered, this, &MainWindow::PrintNow);
    ActionPrintNow->setStatusTip(tr("Print the file Now"));

    FileMenu->addSeparator();
    
    QStringList *HistoryFileList = ReadHistory();
    for( int i = 0; i < HistoryFileList->size(); i++)
    {
        FileMenu->addAction(""+ i + ":" + HistoryFileList[i], this, &MainWindow::OpenRecentFile(HistroyFileList[i]));    
    }
    FileMenu->addSeparator();
    
    FileMenu->addAction("Open All Recent File", this, &MainWindow::OpenAllRecentFile);
    FileMenu->addAction("Clear recent file list", this, &MainWindow::ClearRecentHistory);
    
    FileMenu->addSeparator();
    
    QAction *ActionExit = FileMenu->addAction(tr("Exit"), QKeySequence("Alt + F4"));
    connect(ActionExit, &QAction::triggered, this, &MainWindow::closeEvent);
    ActionExit->setStatusTip(tr("Close the Application"));
    /*--------File Menu End -------*/
    
    
    /*--------Edit Menu Start -------*/
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    connect(editMenu, &QMenu::triggered, this, &MainWindow::RefreshEditMenu);
    ActionUndo = editMenu->addAction(tr("UnDo"), this, &MainWindow::Undo, QKeySequence("ctrl + z"));
    ActionRedo = editMenu->addAction(tr("Redo"), this, &MainWindow::Redo, QKeySequence("ctrl + y"));
    editMenu->addSeparator();

    ActionCut = editMenu->addAction(tr("Cut"), this, &MainWindow::Cut, QKeySequence("ctrl + x"));
    ActionCopy = editMenu->addAction(tr("Copy"), this, &MainWindow::Copy, QKeySequence("ctrl + c"));
    ActionPaste = editMenu->addAction(tr("Paste"), this, &MainWindow::Paste, QKeySequence("ctrl + v"));
    editMenu->addAction(tr("Select All"), this, &MainWindow::SelectAll, QKeySequence("ctrl + a"));
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
    connect(searchMenu, &QMenu::triggered, this, &MainWindow::RefreshSearchMenu);
    searchMenu->addAction(tr("Search"),this, &MainWindow::find, QKeySequence("ctrl + F"));
    searchMenu->addAction(tr("Search in the file"),this, &MainWindow::find, QKeySequence("ctrl + Shift + F"));
    searchMenu->addAction(tr("Find Next"),this, &MainWindow::find, QKeySequence("F3"));
    searchMenu->addAction(tr("Find Previous"),this, &MainWindow::find, QKeySequence("Shift + F3"));
    searchMenu->addAction(tr("Quick Find Next"),this, &MainWindow::find, QKeySequence("ctrl + F3"));
    searchMenu->addAction(tr("Quick Find Previous"),this, &MainWindow::find, QKeySequence("ctrl + Shift + F3"));
    searchMenu->addAction(tr("Replace"),this, &MainWindow::find, QKeySequence("ctrl + H"));
    searchMenu->addAction(tr("Special Research"),this, &MainWindow::find, QKeySequence("ctrl + Alt + I"));
    searchMenu->addAction(tr("Column Locate"),this, &MainWindow::find, QKeySequence("ctrl + G"));
    searchMenu->addAction(tr("Locate pair brace"),this, &MainWindow::find, QKeySequence("ctrl + B"));
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
    GetActiveMdiWindow().document()->undo();
}

void MainWindow::Redo()
{
    GetActiveMdiWindow().document()->redo();
}

void MainWindow::Copy()
{
    GetActiveMdiWindow().copy();
}

void MainWindow::Paste()
{
    GetActiveMdiWindow().paste();
}

void MainWindow::Cut()
{
    GetActiveMdiWindow().cut();
}

void MainWindow::SelectAll()
{
    GetActiveMdiWindow().selectAll();
}

void MainWindow::RefreshEditMenu()
{
    MyMdi *subWindow = GetActiveMdiWindow();
    if(subWindow->document()->isRedoAvailable())
        ActionRedo->setEnabled(true);
    else
        ActionRedo->setEnabled(false);

    if(subWindow->document()->isUndoAvailable())
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
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    QMainWindow::mouseDoubleClickEvent(event);

}

void MainWindow::Find()
{
    TabDialog *dialog = new TabDialog("abc",tabFind, this);
    dialog->setFixedSize(480,320);
    dialog->setWindowIcon(QIcon(QPixmap(0,0)));
    dialog->show();
}

void MainWindow::FindInFile()
{
    TabDialog *dialog = new TabDialog("abc",tabFile, this);
    dialog->setFixedSize(480,320);
    dialog->setWindowIcon(QIcon(QPixmap(0,0)));
    dialog->show();
}

void MainWindow::FindAndReplace()
{
    TabDialog *dialog = new TabDialog("abc",tabReplace, this);
    dialog->setFixedSize(480,320);
    dialog->setWindowIcon(QIcon(QPixmap(0,0)));
    dialog->show();
}

void MainWindow::FindNext()
{
    GetActiveMdiWindow().document()->
}

