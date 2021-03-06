#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "QMdiSubWindow"
#include "QMdiArea"
#include "QMouseEvent"
#include "QSignalMapper"
#include "mydockwidget.h"

class TabDialog;
class QMouseEvent;
class QMdiArea;
class QMdiSubWindow;
class QTextDocument;
class MyMdi;
class QSignalMapper;
class MyDockWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int mdiWindowCnt READ GetmdiWindowCnt WRITE SetmdiWindowCnt NOTIFY mdiWindowCntChanged)
public:
    explicit MainWindow(QWidget *parent = 0);
    void UpdateMenus();
    void CreateFileMenus();
    void CreateEditMenus();
    void CreateSearchMenus();
    void CreateAction();
    MyMdi *FindChildSubWindow(QString filename);
    //void ReadSetting();
    //void WriteSetting();
    //void showRecent();
    void closeEvent(QCloseEvent *event);
    void createStatusBar();
    QStringList ReadHistory();
    void UpdateHistory(QString fileName);
    MyMdi *createSubWindow();
    MyMdi *GetActiveMdiWindow();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void SetmdiWindowCnt(int windowCount);
    int GetmdiWindowCnt();
    void setSearchString();

signals:
    void mdiWindowCntChanged();
public slots:
    void createDefaultMdiWindow();
    /*-----Menu File -----*/
    void RefreshFileMenu();
    void NewFile();
    void OpenFile();
    void Openfile(QString fileName = 0);
    void ReloadFile();
    void SaveAsCopy();
    void SaveCopyText();
    void Save();
    void SaveAs();
    void SaveAll();
    void RenameFile();
    void Close();
    void CloseAll();
    void CloseOthers();
    void DeleteFromDisk();
    void Print();
    void PrintNow();
    void OpenRecentFile(QString fileName = 0);
    void OpenAllRecentFile();
    void ClearRecentHistory();
    void Exit();
    /*-----Menu File -----*/

    /*------Menu Edit-----*/
    void RefreshEditMenu();
    void Undo();
    void Redo();
    void Copy();
    void Paste();
    void Cut();
    void SelectAll();
    /*------Menu Edit-----*/

    /*------Menu Search----*/
    void Find();
    void FindInFile();
    void FindNext();
    void FindNextFromTabDialog(QString searchString, bool matchWholeWord, bool matchUpperLower, bool SearchLoop, bool searchDirection);
    void FindPrev();
    void FindAndReplace();
    void ColumnLocate();
    void TotalCount(bool matchWholeWord, bool matchCaseSencitive);
    //void LocateBrace();
    /*------Menu Search----*/
    void findAllFiles(bool MatchWholeWord, bool MatchUpperLower);
    void gotoLine(QString filePath, int lineNo);
    void setDockWidgetStatus();
    void setWindowsTitle(QMdiSubWindow *subWindow);
    void setTabBarWidth();
    void setSaveActionStatus();
    void setTextToUpper();
    void setTextToLower();
private:
    QMdiArea mdiArea;
    int totalMdiWindowCnt = 0;
    QAction *ActionNew;
    QAction *ActionOpen;
    QAction *ActionReload;
    QAction *ActionSave;
    QAction *ActionSaveAs;
    QAction *ActionSaveCopy;
    QAction *ActionSaveAll;
    QAction *ActionRename;
    QAction *ActionDeleteFile;
    QAction *ActionCloseOtherFiles;
    QAction *ActionCloseAllFile;
    QAction *ActionCloseFile;

    QAction *ActionPrint;
    QAction *ActionPrintNow;
    QAction *ActionExit;

    QAction *ActionUndo;
    QAction *ActionRedo;
    QAction *ActionCut;
    QAction *ActionCopy;
    QAction *ActionPaste;
    QAction *ActionSelectAll;
    QAction *historyAction;
    QAction *separatorBefore;
    QAction *OpenAllRecent;
    QAction *ClearRecent;
    QAction *separatorAfter;

    QString searchString = 0;
    QString prevSearchString = 0;
    QSignalMapper *mapper;
    QString dockWidgetSearchString = 0;

    QMenu *FileMenu;
    QMenu *editMenu;
    QMenu *searchMenu;
    bool IsHistoryCreated = false;
    bool IsDockWidgetInitialized = false;

    enum TabInd{
        tabFind = 0,
        tabReplace = 1,
        tabFile = 2
    };
public:
    TabDialog *dialog;
    MyDockWidget *myDockWidget;
};

#endif // MAINWINDOW_H
