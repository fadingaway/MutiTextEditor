#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "QMdiSubWindow"
#include "QMdiArea"
class QMdiArea;
class QMdiSubWindow;
class MyMdi;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void UpdateMenus();
    void CreateMenus();
    void CreateAction();
    void FindChildSubWindow(QString filename);
    void ReadSetting();
    void WriteSetting();
    void showRecent();
    void closeEvent();
    void createStatusBar();
    QStringList ReadHistory();
    void UpdateHistory(QString fileName);
    MyMdi createSubWindow();
    MyMdi GetActiveMdiWindow();

public slots:
    /*-----Menu File -----*/
    void RefreshFileMenu();
    void NewFile();
    void OpenFile(QString fileName = null);
    void ReloadFile();
    void SaveAsCopy();
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
    void OpenRecentFile(QString fileName);
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
    void FindPrev();
    void FindAndReplace();
    /*------Menu Search----*/

    
    
private:
    QMdiArea mdiArea;
    QAction *ActionNew;
    QAction *ActionOpen;
    QAction *ActionReload;
    QAction *ActionSave;
    QAction *ActionSaveCopy;
    QAction *ActionSaveAll;
    QAction *ActionRename;
    QAction *ActionDeleteFile;

    QAction *ActionUndo;
    QAction *ActionRedo;
    QAction *ActionCut;
    QAction *ActionCopy;
    QAction *ActionPaste;
};

#endif // MAINWINDOW_H
