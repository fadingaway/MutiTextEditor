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
    MyMdi activeMdiWindow();

public slots:
    /*-----Menu File -----*/
    void NewFile();
    void OpenFile();
    void ReloadFile();
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
    void OpenRecentFile(QString fileName);
    void OpenAllRecentFile();
    void ClearRecentHistory();
    void Exit();
    /*-----Menu File -----*/

    /*------Menu Edit-----*/
    void Undo();
    void Redo();
    void Copy();
    void Paste();
    void Cut();
    void Delete();
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
};

#endif // MAINWINDOW_H
